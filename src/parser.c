/**
 * @file parser.c
 * @author Marek Filip <xfilip46>
 * @author Ondřej Míchal <xmicha80>
 * @brief Parser part of ifj20 compiler - the heart of the compiler.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 10/11/2020
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "debug.h"
#include "error.h"
#include "global.h"
#include "parser.h"
#include "scanner.h"
#include "str.h"
#include "symtable.h"
#include "token.h"

scanner_t *scanner; /**< scanner local to the parser */
bool eol_encountered; /**< tracker to tell us if an eol was scanned */
int line = 1; /**< track the current line number */

token_t lookahead; /**< next token to look at */

bool eof_found = false; /**< track if the eof has been found, terminate the analysis correctly */
bool no_eol = false; /**< track if the eol could have been inserted before the token */
bool required_eol = false; /**< track if the eol should be required here */

symtable_t *st; /**< holds symbol stacks for every function */

symtable_t *st_internal_funcs; /** holds signatures of all internal functions */
symtable_t *st_called_funcs; /** holds signatures of all called functions */

// TODO: make eol rules

char *internal_func_names[] = {
    "inputs", "inputi", "inputf",
    "print",
    "int2float", "float2int",
    "len", "substr", "ord", "chr"
};

void parser_move() {
    debug_entry();

    eol_encountered = false;
    // move the lookahead
    int return_code = scanner_scan(scanner, &lookahead, &eol_encountered, &line);

    // TODO: make sure all return cases are handled here

    if (return_code == ERROR_LEXICAL) {
        parser_end(ERROR_LEXICAL);
    } else if (return_code == ERROR_INTERNAL) {
        parser_end(ERROR_INTERNAL);
    } else if (return_code == EOF) {
        eof_found = true;
    }

    if (lookahead.type == INVALID && return_code != EOF) {
        fprintf(stderr, "!!! INVALID TOKEN !!!\n");
        parser_end(ERROR_LEXICAL);
    }

    required_eol = false;
}

void parser_match(token_type t) {
    debug_entry();
    if (lookahead.type == t) {
        debug("Matched type: %s", token_get_type_string(t));
        debug_lit_value(lookahead);

        // EOL CHECKING
        debug("got eol? %d no_eol? %d required_eol? %d", eol_encountered, no_eol, required_eol);
        if (eol_encountered && no_eol) {
            fprintf(stderr, "Wrong EOL placement!\n");
            parser_end(ERROR_SYNTAX);
        } else if (!eol_encountered && required_eol) {
            fprintf(stderr, "EOL should be here!\n");
            parser_end(ERROR_SYNTAX);
        }

        parser_move(); 

    } else {
        debug("Expected type: %s", token_get_type_string(t));
        debug("Got type: %s", token_get_type_string(lookahead.type));
        debug_lit_value(lookahead);

        parser_end(ERROR_SYNTAX);
    }
}

void parser_match_ident(char *ident_name) {
    debug_entry();
    if (lookahead.type == IDENT && strcmp(lookahead.attribute.sym_key, ident_name) == 0) {
        debug("Matched type and string: %s, %s",
                token_get_type_string(IDENT), ident_name);
        parser_move();
    } else {
        debug("Expected type and ident name: %s, %s",
                token_get_type_string(IDENT), ident_name);
        debug("Got type and ident name: %s, %s",
                token_get_type_string(IDENT), lookahead.attribute.sym_key);
        parser_end(ERROR_SYNTAX);
    }
}

/* ------------------------------------------------------------------------ */
/* MAIN RULES                                                               */
/* ------------------------------------------------------------------------ */

void parser_start(scanner_t *scanner_main) {
    debug_entry();

    // Prepare scanner
    scanner = scanner_main;

    // Initialize symtable
    st = symtable_new();
    if (st == NULL)
        parser_end(ERROR_INTERNAL);

    st_internal_funcs = symtable_new();
    if (st_internal_funcs == NULL)
        parser_end(ERROR_INTERNAL);

    st_called_funcs = symtable_new();
    if (st_called_funcs == NULL)
        parser_end(ERROR_INTERNAL);

    const int NUM_OF_FUNCS = 10;
    for (int i = 0; i < NUM_OF_FUNCS; i++) {
        symtable_new_scope(st_internal_funcs, internal_func_names[i]);
    }

    // move the lookahead to the first lexeme
    parser_move();

    parser_prolog();
    parser_funcs();

    parser_end(SUCCESS);
}

void parser_end(int rc) {
    debug_entry();

    // Check if all called functions are defined
    symtable_set_first_scope(st_called_funcs);
    // FIXME: Symtable always starts with an unnamed scope. Until I come up
    // with a proper approach, move to the next scope.
    symtable_next_scope(st_called_funcs);
    do {
        char *func_id = symtable_get_scope_name(st_called_funcs);
        if (rc != SUCCESS) {
            break;  
        }

        func_parameter_t *call_param = symtable_get_func_param(st_called_funcs);

        // Compare every called function identifier with every known function
        // internal functions
        if (symtable_set_current_scope(st_internal_funcs, func_id))
            continue;

        // user-defined functions
        if (symtable_set_current_scope(st, func_id)) {
            // When a match is found, compare the parameters
            func_parameter_t *def_param = symtable_get_func_param(st);

            // Check if parameter types are satisfied
            while (def_param != NULL && call_param != NULL) {
                if (def_param->type != call_param->type) {
                    fprintf(stderr, "Mismatched function parameters\n");
                    rc = ERROR_SEM_PROGRAM;
                    break;
                }

                def_param = def_param->next_param;
                call_param = call_param->next_param;
            }

            // Check if all parameters are satisfied
            if (def_param != NULL || call_param != NULL) {
                if (def_param->type != call_param->type) {
                    fprintf(stderr, "Mismatched function parameters\n");
                    rc = ERROR_SEM_PROGRAM;
                    break;
                }
            }

            continue;
        }

        fprintf(stderr, "Function %s is not defined\n", func_id);
        rc = ERROR_SEM_VAR;
    } while (symtable_next_scope(st_called_funcs));

    symtable_free(st);
    symtable_free(st_called_funcs);
    symtable_free(st_internal_funcs);

    if (rc == ERROR_LEXICAL)
        throw_lex_error(line);
    else if (rc == ERROR_SYNTAX)
        throw_syntax_error(lookahead.type, line);
    else if (rc >= ERROR_SEM_VAR && rc <= ERROR_ZERO)
        throw_semantics_error(rc, line);
    else if (rc == ERROR_INTERNAL)
        throw_internal_error();
}

void parser_prolog() {
    debug_entry();
    parser_match(PACKAGE);
    parser_match_ident("main"); 
}

void parser_funcs() {
    debug_entry();

    if (eof_found) {
        fprintf(stderr, "EOF encountered, stopping syntax analysis...\n");
        fprintf(stderr, "line: %d\n", line);
        return;
    }

    // if FUNC not found, apply eps-rule
    if (lookahead.type == FUNC) {
        // function decleration
        // New function -> new local scope of symbols
        parser_match(FUNC); 

        char *func_id = lookahead.attribute.sym_key;
        no_eol = true;
        parser_match(IDENT);

        debug("New local scope - %s", func_id);
        if (!symtable_new_scope(st, func_id)) {
            fprintf(stderr, "Function with name %s already exists\n", func_id);
            parser_end(ERROR_SEM_VAR);
        }

        parser_params();
        symtable_push_stack(st);
        parser_r_params();
        parser_block();
        parser_funcs();
    }
    // apply eps-rule

    // if EOF not found, throw syntax error, program should not end here.
    if (!eof_found)
        parser_end(ERROR_SYNTAX);
}

void parser_stmts() {
    debug_entry();
    switch (lookahead.type) {
        case IDENT:
        case IF:
        case FOR:
        case RETURN:
            parser_stmt();
            parser_stmts();
            break;
        default:
            // apply eps-rule
            break;
    }
}

void parser_stmt() {
    debug_entry();
    char *ident = lookahead.attribute.sym_key;
    switch (lookahead.type) {
        case IDENT:
            // identifier found
            parser_match(IDENT);
            parser_id_first(ident);
            break;
        case IF:
            // if statement
            parser_stmt_if();
            break;
        case FOR:
            // for loop
            parser_stmt_for();
            break;
        case RETURN:
            // return statement
            parser_return();
            required_eol = true;
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    }
}

void parser_id_first(char *id) {
    debug_entry();

    switch (lookahead.type) {
        case DEFINE:
            // variable definition
            parser_vardef(id);
            required_eol = true;
            break;
        case ASSIGN:
        case COMMA:
            // variable assign
            parser_assign(id);
            required_eol = true;
            break;
        case LPAREN:
            // function call
            // Store identifier of the called function
            parser_func_call(id);
            required_eol = true;
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    }
}

/* ------------------------------------------------------------------------ */
/* GENERAL HELP RULES                                                       */
/* ------------------------------------------------------------------------ */

void parser_type() {
    debug_entry();
    switch (lookahead.type) {
        case INT:
            parser_match(INT);
            break;
        case FLOAT64:
            parser_match(FLOAT64);
            break;
        case STRING:
            parser_match(STRING);
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    }
}

void parser_block() {
    debug_entry();
    parser_match(LBRACE);
    no_eol = false;
    parser_stmts();
    parser_match(RBRACE);
}

/* ------------------------------------------------------------------------ */
/* STATEMENT RULES                                                          */
/* ------------------------------------------------------------------------ */

void parser_stmt_if() {
    debug_entry();

    parser_match(IF);

    symtable_push_stack(st);

    no_eol = true;
    parser_expr();
    parser_block();
    parser_match(ELSE);
    no_eol = true;
    parser_block();
    required_eol = true;

    symtable_push_stack(st);
}

void parser_stmt_for() {
    debug_entry();

    parser_match(FOR);

    symtable_push_stack(st);

    no_eol = true;
    parser_optdef();
    parser_match(SEMICOLON);
    parser_expr();
    parser_match(SEMICOLON);
    parser_optassign();
    parser_block();
    required_eol = true;

    symtable_push_stack(st);
}

/* ------------------------------------------------------------------------ */
/* PARAMETERS OF FUNCTION RULES                                             */
/* ------------------------------------------------------------------------ */

void parser_params() {
    debug_entry();

    parser_match(LPAREN);

    switch (lookahead.type) {
        case IDENT:
            parser_param();
            parser_match(RPAREN);
            break;
        case RPAREN:
            parser_match(RPAREN);
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    } 
}

void parser_param() {
    debug_entry();

    char *id = lookahead.attribute.sym_key;
    parser_match(IDENT);

    token_type type = lookahead.type;
    parser_type();

    symtable_add_func_param(st, id, type);

    parser_param_n();
}

void parser_param_n() {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        parser_track_ident(lookahead.attribute.sym_key);
        symtable_add_func_param(st, lookahead.attribute.sym_key, lookahead.type);
        no_eol = false;
        parser_match(IDENT);

        no_eol = true;
        parser_type();
        parser_param_n();
    }
    // apply eps-rule
}

void parser_r_params() {
    debug_entry();
    // if '(' not found, apply eps-rule
    if (lookahead.type == LPAREN) {
        parser_match(LPAREN);

        switch (lookahead.type) {
            // depending on the case either put ')' or continue with params
            case INT:
            case FLOAT64:
            case STRING:
                parser_r_param();
                parser_match(RPAREN);
                break;
            case RPAREN:
                parser_match(RPAREN);
                break;
            default:
                // no eps-rule -> throw syntax error
                parser_end(ERROR_SYNTAX);
                break;
        }
    }
    // apply eps-rule
}

void parser_r_param() {
    debug_entry();

    symtable_add_func_return(st, lookahead.type);
    parser_type();
    parser_r_param_n();
}

void parser_r_param_n() {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);
        symtable_add_func_return(st, lookahead.type);
        parser_type();
        parser_r_param_n();
    }
    // apply eps-rule
}

void parser_func_call(char *id) {
    debug_entry();

    symtable_new_scope(st_called_funcs, id);

    // Assemble all parameters
    func_parameter_t *first_parameter = symtable_get_func_param(st_called_funcs);;
    parser_c_params(first_parameter);
}

void parser_c_params(func_parameter_t *first_param) {
    debug_entry();
    parser_match(LPAREN);

    switch (lookahead.type) {
        case LPAREN:
        case INT_LIT:
        case FLOAT64_LIT:
        case STRING_LIT:
        case IDENT:
            parser_c_param(first_param);
            parser_match(RPAREN);
            break;
        case RPAREN:
            parser_match(RPAREN);
            break;
        default:
            // no eps-rule -> throw syntax error
            parser_end(ERROR_SYNTAX);
            break;
    }
}

void parser_c_param(func_parameter_t *param) {
    debug_entry();
    // Resolve expression
    parser_expr();
    param->next_param = func_parameter_new();
    if (param->next_param == NULL)
        parser_end(ERROR_INTERNAL);

    parser_c_param_n(param->next_param);
}

void parser_c_param_n(func_parameter_t *param) {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        param->next_param = func_parameter_new();
        if (param->next_param == NULL)
            parser_end(ERROR_INTERNAL);

        // Resolve expression
        parser_expr();
        parser_c_param_n(param->next_param);
    }
    // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* VAR DEFINITION AND ASSIGN                                                */
/* ------------------------------------------------------------------------ */

void parser_vardef(char *id) {
    debug_entry();

    parser_track_ident(id);
    parser_match(DEFINE);

    // TODO: Resolve expression
    parser_expr();
}

void parser_assign(char *id) {
    debug_entry();

    int num_of_assigns = 0;

    if (strcmp(id, "_") != 0 && symtable_find_symbol(st, id) == NULL) {
        fprintf(stderr, "Variable %s is not defined\n", id);
        parser_end(ERROR_SEM_VAR);
    }

    if (strcmp(id, "_") != 0)
        num_of_assigns++;

    parser_id_n(&num_of_assigns);
    parser_match(ASSIGN);
    parser_exprs(&num_of_assigns);

    // TODO: Resolve expressions

    if (num_of_assigns != 0) {
        fprintf(stderr, "Mismatched number of assigned values\n");
        parser_end(ERROR_SEM_PROGRAM);
    }
}

void parser_id_n(int *num_of_ids) {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        char *id = lookahead.attribute.sym_key;
        if (strcmp(id, "_") != 0 && symtable_find_symbol(st, id) == NULL) {
            fprintf(stderr, "Variable %s is not defined\n", id);
            parser_end(ERROR_SEM_VAR);
        }

        if (strcmp(id, "_") != 0)
            (*num_of_ids)++;

        parser_match(IDENT);
        parser_id_n(num_of_ids);
    }
    // apply eps-rule
}

void parser_exprs(int *num_of_exprs) {
    debug_entry();

    parser_expr();
    (*num_of_exprs)--;

    parser_expr_n(num_of_exprs);
}

void parser_expr_n(int *num_of_exprs) {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);
        parser_expr();
        (*num_of_exprs)--;
        parser_expr_n(num_of_exprs);
    }
    // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* HELPER FOR RULES                                                         */
/* ------------------------------------------------------------------------ */

void parser_optdef() {
    debug_entry();
    // if IDENT not found, apply eps-rule
    if (lookahead.type == IDENT) {
        char *id = lookahead.attribute.sym_key;

        parser_match(IDENT);
        parser_vardef(id);
    }
    // apply eps-rule
}

void parser_optassign() {
    debug_entry();
    // if IDENT not found, apply eps-rule
    if (lookahead.type == IDENT) {
        char *id = lookahead.attribute.sym_key;

        parser_match(IDENT);
        parser_assign(id);
    }
    // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* RETURN STATEMENT                                                         */
/* ------------------------------------------------------------------------ */

// return 10, 34
// return
// eps
void parser_return() {
    debug_entry();
    parser_match(RETURN);  

    int num_of_returns = symtable_get_num_of_returns(st);

    parser_optexprs(&num_of_returns);

    if (num_of_returns != 0) {
        fprintf(stderr, "Wrong number of returned values\n");
        parser_end(ERROR_SEM_PROGRAM);
    }
}

void parser_optexprs(int *num_of_exprs) {
    debug_entry();
    // optional expression depending on the type read
    switch (lookahead.type) {
        case LPAREN:
        case INT_LIT:
        case FLOAT64_LIT:
        case STRING_LIT:
        case IDENT:
            parser_exprs(num_of_exprs);
            break;
        default:
            // apply eps-rule
            break;
    }
}

/* ------------------------------------------------------------------------ */
/* EXPRESSION RULES                                                         */
/* ------------------------------------------------------------------------ */

void parser_expr() {
    // This parsing is only used for the 0th submission
    debug_entry();  
    parser_rel();
}

/* THIS WILL BE LEFT OUT IN THE FINAL PARSER */

/* Helper functions */
bool is_relop(token_type type) {
    // Does token type represent relation operations?
    switch (type) {
        case LSS:
        case LEQ:
        case GTR:
        case GEQ:
        case EQL:
        case NEQ:
            return true;
        default:
            return false;
    }
}

bool is_addop(token_type type) {
    // Does token type represent plus/minus opperations?
    switch (type) {
        case ADD:
        case SUB:
            return true;
        default:
            return false;
    }
}

bool is_mulop(token_type type) {
    // Does token type represent multiply/divide operations?
    switch (type) {
        case MUL:
        case DIV:
            return true;
        default:
            return false;
    }
}

void parser_relop() {
    debug_entry();
    if (is_relop(lookahead.type)) {
        // we can call 'match' like that because of the check in the if 
        parser_match(lookahead.type);
    } else {
        // operator not found, syntax error
        throw_syntax_error(lookahead.type, line);
    }
}

void parser_addop() {
    debug_entry();
    if (is_addop(lookahead.type)) {
        // we can call 'match' like that because of the check in the if 
        parser_match(lookahead.type);
    } else {
        // operator not found, syntax error
        throw_syntax_error(lookahead.type, line);
    }
}

void parser_mulop() {
    debug_entry();
    if (is_mulop(lookahead.type)) {
        // we can call 'match' like that because of the check in the if 
        parser_match(lookahead.type);
    } else {
        // operator not found, syntax error
        throw_syntax_error(lookahead.type, line);
    }
}

void parser_rel() {
    debug_entry();
    parser_add();
    parser_rel_n();
}

void parser_rel_n() {
    debug_entry();
    // if token type is not relation operator, apply eps-rule
    if (is_relop(lookahead.type)) {
        // expand the relation operator
        parser_relop();
        parser_add();
        parser_rel_n();
    }
    // apply eps-rule
}

void parser_add() {
    debug_entry();
    parser_term();
    parser_add_n();
}

void parser_add_n() {
    debug_entry();
    // if token type is not add/sub perator, apply eps-rule
    if (is_addop(lookahead.type)) {
        parser_addop();
        parser_term();
        parser_add_n();
    }
    // apply eps-rule
}

void parser_term() {
    debug_entry();
    parser_factor();
    parser_term_n();
}

void parser_term_n() {
    debug_entry();
    // if token type is not mul/div operator, apply eps-rule
    if (is_mulop(lookahead.type)) {
        parser_mulop();
        parser_factor();
        parser_term_n();
    }
    // apply eps-rule
}

void parser_factor() {
    debug_entry();
    token_type t = lookahead.type;
    char *id = lookahead.attribute.sym_key;

    if (t == LPAREN) {
        parser_match(LPAREN);
        parser_expr();
        parser_match(RPAREN);
    } else if (token_is_lit(t)) {
        // can only match INT, FLOAT64, STRING due to is_lit()
        parser_match(t);
    } else if (t == IDENT) {
        parser_match(IDENT);
        t = lookahead.type;
        if (t == LPAREN)
            parser_func_call(id);
    } else {
        throw_syntax_error(lookahead.type, line);
    }
}

/* ------------------------------------------------------------------------ */
/* SEMANTIC ACTIONS                                                         */
/* ------------------------------------------------------------------------ */

void parser_track_ident(char *id) {
    debug_entry();  
    debug("Adding symbol %s to symtable", id);

    symtable_symbol_t *symbol = symtable_add_symbol(st, id);

    symbol->type = IDENT;
}
