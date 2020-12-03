/**
 * @file parser.c
 * @author Marek Filip <xfilip46>
 * @author Ondřej Míchal <xmicha80>
 * @brief Parser part of ifj20 compiler - the heart of the compiler.
 * @details Implementace překladače imperativního jazyka IFJ20.
 * @date 10/11/2020
 */

#include <string.h>

#include "debug.h"
#include "error.h"
#include "global.h"
#include "parser.h"
#include "scanner.h"
#include "stack_charptr.h"
#include "stack_func_call_t.h"
#include "stack_int.h"
#include "stack_token_t.h"
#include "symtable.h"
#include "str.h"
#include "three-address-code.h"

scanner_t *scanner; /**< scanner local to the parser */
bool eol_encountered; /**< tracker to tell us if an eol was scanned */
int line = 1; /**< track the current line number */

token_t lookahead; /**< next token to look at */

bool eof_found = false; /**< track if the eof has been found, terminate the analysis correctly */
bool no_eol = false; /**< track if the eol could have been inserted before the token */
bool required_eol = false; /**< track if the eol should be required here */

symtable_t *st; /**< holds symbol stacks for every function; this is the main symtable in parser */
symtable_t *st_internal_funcs; /**< holds signatures of all internal functions */
stack_func_call_t *stack_func_calls; /**< holds info about every function call */

int temp_var_cnt;  /**< temporary variable's index tracker */
int label_cnt;  /**< labels's index tracker */

stack_charptr_t *stack_temp_vars; /**< stack of ids of temporary variables needed for expressions */
stack_charptr_t *stack_assigns; /**< stack of variables where the result of an expression is saved */

int max_func_returns; /**< max number of returns that a function call can return in an expression */
stack_int_t *stack_func_ret_types; /**< stack of types a function call should return in an expression */
bool expr_has_sus_func_call; /**< an indicator if an expression has a function call */

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

    // Initialize symbol tables for variables and functions
    st = symtable_new();
    if (st == NULL)
        parser_end(ERROR_INTERNAL);

    st_internal_funcs = symtable_new();
    if (st_internal_funcs == NULL)
        parser_end(ERROR_INTERNAL);

    char *internal_func_names[] = {
        "inputs", "inputi", "inputf",
        "print",
        "int2float", "float2int",
        "len", "substr", "ord", "chr"
    };
    const int NUM_OF_FUNCS = 10;
    for (int i = 0; i < NUM_OF_FUNCS; i++) {
        symtable_new_scope(st_internal_funcs, internal_func_names[i]);
        switch(i) {
            case 0:
                symtable_add_func_return(st_internal_funcs, STRING_LIT);
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                break;
            case 1:
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                break;
            case 2:
                symtable_add_func_return(st_internal_funcs, FLOAT64_LIT);
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                break;
            case 3:
                // print()
                symtable_add_inf_func_param(st_internal_funcs);
                break;
            case 4:
                symtable_add_func_param(st_internal_funcs, "i", INT_LIT);
                symtable_add_func_return(st_internal_funcs, FLOAT64_LIT);
                break;
            case 5:
                symtable_add_func_param(st_internal_funcs, "f", FLOAT64_LIT);
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                break;
            case 6:
                symtable_add_func_param(st_internal_funcs, "s", STRING_LIT);
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                break;
            case 7:
                symtable_add_func_param(st_internal_funcs, "s", STRING_LIT);
                symtable_add_func_param(st_internal_funcs, "i", INT_LIT);
                symtable_add_func_param(st_internal_funcs, "n", INT_LIT);
                symtable_add_func_return(st_internal_funcs, STRING_LIT);
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                break;
            case 8:
                symtable_add_func_param(st_internal_funcs, "s", STRING_LIT);
                symtable_add_func_param(st_internal_funcs, "i", INT_LIT);
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                break;
            case 9:
                symtable_add_func_param(st_internal_funcs, "i", INT_LIT);
                symtable_add_func_return(st_internal_funcs, STRING_LIT);
                symtable_add_func_return(st_internal_funcs, INT_LIT);
                break;
        }
    }

    stack_temp_vars = stack_charptr_init();
    if (stack_temp_vars == NULL)
        parser_end(ERROR_INTERNAL);

    stack_assigns = stack_charptr_init();
    if (stack_assigns == NULL)
        parser_end(ERROR_INTERNAL);

    stack_func_calls = stack_func_call_t_init();
    if (stack_func_calls == NULL)
        parser_end(ERROR_INTERNAL);

    // move the lookahead to the first lexeme
    parser_move();

    parser_prolog();
    parser_funcs();

    parser_end(SUCCESS);
}

void parser_end(int rc) {
    debug_entry();

    // Check if all called functions are defined and called properly
    debug("STACK FUNC CALLS TOP: %d", stack_func_calls->top);
    while(stack_func_calls->top >= 0) {
        func_parameter_t *def_param, *call_param = NULL;
        func_return_t *def_return = NULL;
        char *func_id;
        int num_of_def_params, num_of_def_returns;

        if (rc != SUCCESS)
            break;
        
        func_call_t fc = stack_func_call_t_pop(stack_func_calls);
        // Get called function identifier
        func_id = fc.func.id.str;
        // Get called functino first parameter
        call_param = fc.func.first_parameter;

        // Look for the called function in all known functions: internal and
        // user-defined functions. If a function is not found, exit with
        // semantics error.
        if (symtable_set_current_scope(st_internal_funcs, func_id)) {
            def_param = symtable_get_func_param(st_internal_funcs);
            num_of_def_params = symtable_get_num_of_params(st_internal_funcs);
            def_return = symtable_get_func_return(st_internal_funcs);
            num_of_def_returns = symtable_get_num_of_returns(st_internal_funcs);
        } else if (symtable_set_current_scope(st, func_id)) {
            def_param = symtable_get_func_param(st);
            num_of_def_params = symtable_get_num_of_params(st);
            def_return = symtable_get_func_return(st);
            num_of_def_returns = symtable_get_num_of_returns(st);
        } else {
            fprintf(stderr, "Function %s is not defined\n", func_id);
            rc = ERROR_SEM_VAR;
            break;
        }

        debug("Called function: %s; num of returns: %d, num of parameters: %d", func_id, fc.num_of_expected_returns, fc.func.num_of_parameters);
        debug("Defined function; num of returns: %d, num of parameters: %d", num_of_def_returns, num_of_def_params);

        // Check if parameters are satisfied
        if (fc.func.num_of_parameters != num_of_def_params && num_of_def_params != -1) {
            fprintf(stderr, "Mismatched number of function parameters\n");
            rc = ERROR_SEM_PROGRAM;
            break;
        }

        while (num_of_def_params != -1 && (def_param != NULL || call_param != NULL)) {
            debug("DEF PARAM: %s, CALL PARAM: %s", token_get_type_string(def_param->type), token_get_type_string(call_param->type));
            if (def_param->type != call_param->type) {
                fprintf(stderr, "Mismatched function parameter type\n");
                rc = ERROR_SEM_PROGRAM;
                break;
            }

            def_param = def_param->next_param;
            call_param = call_param->next_param;
        }

        if (rc != SUCCESS)
            break;

        // Check if returns are correct
        if (fc.num_of_expected_returns != num_of_def_returns) {
            fprintf(stderr, "Mismatched number of returned values\n");
            rc = ERROR_SEM_PROGRAM;
            break;
        }

        while (fc.first_assign != NULL && def_return != NULL) {
            if (fc.first_assign->type != INVALID && fc.first_assign->type != def_return->type) {
                fprintf(stderr, "Mismatched return value type\n");
                rc = ERROR_SEM_PROGRAM;
                break;
            }

            fc.first_assign = fc.first_assign->next_assign;
            def_return = def_return->next_return;
        }
    } 

    symtable_free(st);
    symtable_free(st_internal_funcs);
    stack_charptr_free(stack_assigns);
    clean_temporary_variables();
    stack_charptr_free(stack_temp_vars);
    clean_func_calls();
    stack_func_call_t_free(stack_func_calls);

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

        // Create function label
        TAC_insert(tac_list, OP_LABEL_FUNC, NULL , NULL, func_id);

        debug("New local scope - %s", func_id);
        if (!symtable_new_scope(st, func_id)) {
            fprintf(stderr, "Function with name %s already exists\n", func_id);
            parser_end(ERROR_SEM_VAR);
        }

        symtable_push_stack(st);
        parser_params();
        parser_r_params();
        parser_block();
        TAC_insert(tac_list, OP_RETURN, NULL, NULL, NULL);
        parser_funcs();
    }
    // apply eps-rule

    // if EOF not found, throw syntax error, program should not end here.
    if (!eof_found) {
        parser_end(ERROR_SYNTAX);
    }
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
            break; default: // apply eps-rule
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
            parser_assign(id, tac_list);
            required_eol = true;
            break;
        case ADD_ASSIGN:
            // += assign
            parser_add_assign(id, tac_list);
            break;
        case SUB_ASSIGN:
            // -= assign
            parser_sub_assign(id, tac_list);
            break;
        case MUL_ASSIGN:
            // *= assign
            parser_mul_assign(id, tac_list);
            break;
        case DIV_ASSIGN:
            // /= assign
            parser_div_assign(id, tac_list);
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

// my_list needed for the for statement
void parser_add_assign(char *id, TACList *my_list) {
    parser_match(ADD_ASSIGN);
    parser_expr();
    required_eol = true;

    char *res_string = (char *)stack_charptr_pop(stack_temp_vars);
    symtable_symbol_t *res = symtable_find_symbol(st, res_string);
    char *expr = TAC_get_identifier(res_string);
    
    symtable_symbol_t *target_sym = symtable_find_symbol(st, id);
    char *target_id = TAC_get_identifier(id);

    if (res->token.type != FLOAT64_LIT && res->token.type != INT_LIT &&
            res->token.type != STRING_LIT && res->token.type != target_sym->token.type) {
        parser_end(ERROR_SEM_COMPATIBILITY);
    }

    TAC_insert(my_list, OP_ADD_ASSIGN, target_id, expr, target_id);
}

// my_list needed for the for statement
void parser_sub_assign(char *id, TACList *my_list) {
    parser_match(SUB_ASSIGN);
    parser_expr();
    required_eol = true;

    char *res_string = (char *)stack_charptr_pop(stack_temp_vars);
    symtable_symbol_t *res = symtable_find_symbol(st, res_string);
    char *expr = TAC_get_identifier(res_string);
    
    symtable_symbol_t *target_sym = symtable_find_symbol(st, id);
    char *target_id = TAC_get_identifier(id);

    if (res->token.type != FLOAT64_LIT && res->token.type != INT_LIT &&
            res->token.type != STRING_LIT && res->token.type != target_sym->token.type) {
        parser_end(ERROR_SEM_COMPATIBILITY);
    }

    TAC_insert(my_list, OP_SUB_ASSIGN, target_id, expr, target_id);
}

// my_list needed for the for statement
void parser_mul_assign(char *id, TACList *my_list) {
    parser_match(MUL_ASSIGN);
    parser_expr();
    required_eol = true;

    char *res_string = (char *)stack_charptr_pop(stack_temp_vars);
    symtable_symbol_t *res = symtable_find_symbol(st, res_string);
    char *expr = TAC_get_identifier(res_string);
    
    symtable_symbol_t *target_sym = symtable_find_symbol(st, id);
    char *target_id = TAC_get_identifier(id);

    if (res->token.type != FLOAT64_LIT && res->token.type != INT_LIT &&
            res->token.type != target_sym->token.type) {
        parser_end(ERROR_SEM_COMPATIBILITY);
    }

    TAC_insert(my_list, OP_MUL_ASSIGN, target_id, expr, target_id);
}

// my_list needed for the for statement
void parser_div_assign(char *id, TACList *my_list) {
    parser_match(DIV_ASSIGN);
    parser_expr();
    required_eol = true;

    // TODO: if float IDIV, if int DIV
    char *res_string = (char *)stack_charptr_pop(stack_temp_vars);
    symtable_symbol_t *res = symtable_find_symbol(st, res_string);
    char *expr = TAC_get_identifier(res_string);
    
    symtable_symbol_t *target_sym = symtable_find_symbol(st, id);
    char *target_id = TAC_get_identifier(id);

    operator_type op = OP_INVALID;

    if (res->token.type != target_sym->token.type) {
        parser_end(ERROR_SEM_COMPATIBILITY);
    }
    
    if (res->token.type == FLOAT64_LIT) {
        op = OP_IDIV_ASSIGN;
        if (res->token.attribute.float_val == 0) {
            parser_end(ERROR_ZERO);
        }
    } else if (res->token.type == INT_LIT) {
        op = OP_DIV_ASSIGN;
        if (res->token.attribute.int_val == 0) {
            parser_end(ERROR_ZERO);
        }
    } else {
        // FIXME: correct semantic error
        parser_end(ERROR_SEM_COMPATIBILITY);
    }

    TAC_insert(my_list, op, target_id, expr, target_id);
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
    required_eol = true;
    parser_stmts();
    parser_match(RBRACE);
}

/* ------------------------------------------------------------------------ */
/* STATEMENT RULES                                                          */
/* ------------------------------------------------------------------------ */

void parser_stmt_if() {
    debug_entry();

    parser_match(IF);

    char *else_label = create_label_name();
    char *if_end_label = create_label_name();

    symtable_push_stack(st);

    no_eol = true;
    parser_expr();  // bottom-up expression analysis

    char *res_ident = (char *)stack_charptr_pop(stack_temp_vars);
    symtable_symbol_t *res = symtable_find_symbol(st, res_ident);
    if (res->token.type != BOOL_LIT) {
        fprintf(stderr, "If statement conditional expression has to be bool");
        parser_end(ERROR_SEM);
    }

    TAC_insert(tac_list, OP_JUMPIFNEQ, TAC_get_identifier(res_ident), "btrue", else_label);

    // if label
    TAC_insert(tac_list, OP_INC_SCOPE, NULL, NULL, NULL);

    parser_block();
    parser_match(ELSE);

    TAC_insert(tac_list, OP_JUMP, NULL, NULL, if_end_label);

    // else label
    TAC_insert(tac_list, OP_LABEL, NULL, NULL, else_label);

    no_eol = true;
    parser_block();
    required_eol = true;

    // if-end label
    TAC_insert(tac_list, OP_LABEL, NULL, NULL, if_end_label);
    TAC_insert(tac_list, OP_DEC_SCOPE, NULL, NULL, NULL);

    symtable_pop_stack(st);
}

void parser_stmt_for() {
    debug_entry();

    parser_match(FOR);

    char *for_end_label = create_label_name();
    char *condition_label = create_label_name();

    TAC_insert(tac_list, OP_INC_SCOPE, NULL, NULL, NULL);
    // "header" of for loop has it's own stack of symbols
    symtable_push_stack(st);
    no_eol = true;
    // i := 0
    parser_optdef();
    parser_match(SEMICOLON);

    parser_expr();  // e.g. i < 10
    char *res_ident = (char *)stack_charptr_pop(stack_temp_vars);
    symtable_symbol_t *res = symtable_find_symbol(st, res_ident);
    if (res->token.type != BOOL_LIT) {
        fprintf(stderr, "For statement conditional expression has to be bool");
        parser_end(ERROR_SEM);
    }

    parser_match(SEMICOLON);

    // Test the condition label
    TAC_insert(tac_list, OP_LABEL, NULL, NULL, condition_label);
    TAC_insert(tac_list, OP_JUMPIFNEQ, TAC_get_identifier(res_ident), "btrue", for_end_label);

    // save the three address code instructions for later
    TACList *tac_optassign_list = TAC_new();
    TAC_create_row(tac_optassign_list);

    parser_optassign(tac_optassign_list);

    TAC_insert(tac_list, OP_INC_SCOPE, NULL, NULL, NULL);
    symtable_push_stack(st);
    parser_block();
    required_eol = true;
    TAC_insert(tac_list, OP_DEC_SCOPE, NULL, NULL, NULL);
    symtable_pop_stack(st);

    // iteration label, e.g. i += 1
    TACptr curr = tac_optassign_list->first;
    while (curr != NULL) {
        TAC_insert(tac_list, curr->op, curr->arg1, curr->arg2, curr->result);
        curr = curr->next_row;
    }
    TAC_free(tac_optassign_list);
    TAC_insert(tac_list, OP_JUMP, NULL, NULL, condition_label);

    TAC_insert(tac_list, OP_LABEL, NULL, NULL, for_end_label);
    TAC_insert(tac_list, OP_DEC_SCOPE, NULL, NULL, NULL);
    symtable_pop_stack(st);
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
    no_eol = true;
    parser_match(IDENT);

    token_type type = lookahead.type;
    no_eol = true;
    parser_type();

    parser_track_ident(id);
    symtable_symbol_t *arg = symtable_find_symbol(st, id);
    switch (type) {
    case INT:
        arg->token.type = INT_LIT;
        break;
    case FLOAT64:
        arg->token.type = FLOAT64_LIT;
        break;
    case STRING:
        arg->token.type = STRING_LIT;
        break;
    default:
        fprintf(stderr, "Unsupported type\n");
        parser_end(ERROR_SEM_COMPATIBILITY);
        break;
    }
    
    symtable_add_func_param(st, id, arg->token.type);

    int arg_num = 1;
    TAC_insert(tac_list, OP_MOVE, TAC_create_function_argument(arg_num), NULL, TAC_get_identifier(id));

    parser_param_n(++arg_num);
}

void parser_param_n(int arg_num) {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        char *id = lookahead.attribute.sym_key;
        no_eol = false;
        parser_match(IDENT);

        token_type type = lookahead.type;
        no_eol = true;
        parser_type();

        parser_track_ident(id);
        symtable_symbol_t *arg = symtable_find_symbol(st, id);
        switch (type) {
        case INT:
            arg->token.type = INT_LIT;
            break;
        case FLOAT64:
            arg->token.type = FLOAT64_LIT;
            break;
        case STRING:
            arg->token.type = STRING_LIT;
            break;
        default:
            fprintf(stderr, "Unsupported type\n");
            parser_end(ERROR_SEM_COMPATIBILITY);
            break;
        }

        symtable_add_func_param(st, id, arg->token.type);

        TAC_insert(tac_list, OP_MOVE, TAC_create_function_argument(arg_num), NULL, TAC_get_identifier(id));

        parser_param_n(++arg_num);
    }
    // apply eps-rule
}

void parser_r_params() {
    debug_entry();
    // if '(' not found, apply eps-rule
    debug("LOOKING FOR RETURN");
    if (lookahead.type == LPAREN) {
        debug("FOUND RETURN");
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

    token_type ret_type = lookahead.type;
    parser_type();

    switch (ret_type) {
    case INT:
        ret_type = INT_LIT;
        break;
    case FLOAT64:
        ret_type = FLOAT64_LIT;
        break;
    case STRING:
        ret_type = STRING_LIT;
        break;
    default:
        fprintf(stderr, "Unexpected return type\n");
        parser_end(ERROR_SEM_PROGRAM);
        break;
    }
    symtable_add_func_return(st, ret_type);

    parser_r_param_n();
}

void parser_r_param_n() {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        token_type ret_type = lookahead.type;
        parser_type();
        no_eol = true;
        
        switch (ret_type) {
        case INT:
            ret_type = INT_LIT;
            break;
        case FLOAT64:
            ret_type = FLOAT64_LIT;
            break;
        case STRING:
            ret_type = STRING_LIT;
            break;
        default:
            fprintf(stderr, "Unexpected return type\n");
            parser_end(ERROR_SEM_PROGRAM);
            break;
        }
        symtable_add_func_return(st, ret_type);

        parser_r_param_n();
    }
    // apply eps-rule
}

void parser_func_call(char *id) {
    debug_entry();

    // TODO: Check if a variable with the same id exists in the same scope

    // symtable_new_scope(st_called_funcs, id);
    // Track function call for later evaluation
    func_call_t fc;
    func_call_init(&fc);
    func_set_id(&fc.func, id);

    bool is_print = strcmp(id, "print") == 0;

    if (!is_print)
        TAC_insert(tac_list, OP_CREATE_FRAME, NULL, NULL, NULL);

    // Assemble all parameters
    parser_c_params(&fc.func);

    stack_func_call_t_push(stack_func_calls, fc);

    // print() is implemented as an "inline" function in code generation. It
    // has to be called for every parameter separately.
    func_parameter_t *param = fc.func.first_parameter;
    unsigned arg_num = 1;
    while (param != NULL) {
        char *arg = TAC_create_function_argument(arg_num);

        if (is_print) {
            TAC_insert(tac_list, OP_PRINT, TAC_get_identifier(param->id.str), NULL, NULL);
        } else {
            TAC_insert(tac_list, OP_DEFINE, NULL, NULL, arg);
            TAC_insert(tac_list, OP_MOVE, TAC_get_identifier(param->id.str), NULL, TAC_get_identifier(arg));
        }

        arg_num++;
        param = param->next_param;
    }

    if (!is_print)
        TAC_insert(tac_list, OP_CALL, id, NULL, NULL);
}

void parser_c_params(func_t *func) {
    debug_entry();
    parser_match(LPAREN);

    switch (lookahead.type) {
        case LPAREN:
        case INT_LIT:
        case FLOAT64_LIT:
        case STRING_LIT:
        case IDENT:
            parser_c_param(func);
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

void parser_c_param(func_t *func) {
    debug_entry();
    // Resolve expression
    parser_expr();

    char *arg_ident = (char *) stack_charptr_pop(stack_temp_vars);
    symtable_symbol_t *arg = symtable_find_symbol(st, arg_ident);
    func_add_parameter(func, arg_ident, arg->token.type);

    parser_c_param_n(func);
}

void parser_c_param_n(func_t *func) {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        // Resolve expression
        parser_expr();

        char *arg_ident = (char *) stack_charptr_pop(stack_temp_vars);
        symtable_symbol_t *arg = symtable_find_symbol(st, arg_ident);
        func_add_parameter(func, arg_ident, arg->token.type);

        parser_c_param_n(func);
    }
    // apply eps-rule
}

/* ------------------------------------------------------------------------ */
/* VAR DEFINITION AND ASSIGN                                                */
/* ------------------------------------------------------------------------ */

void parser_vardef(char *id) {
    debug_entry();

    // TODO: Check if redefine is happening
    // symtable_find_symbol - omezit boolem na stack
    if (symtable_find_symbol_curr_scope(st, id)) {
        fprintf(stderr, "Redefinition of a variable is FORBIDDEN!");
        parser_end(ERROR_SEM_VAR);
    }

    parser_track_ident(id);
    symtable_symbol_t *symbol = symtable_find_symbol(st, id);
    parser_match(DEFINE);

    stack_charptr_push(stack_assigns, id);
    max_func_returns = 1;
    parser_expr();

    stack_charptr_pop(stack_assigns);

    // The number of returned temporary variables should be equal to 1. We can
    // define only a single variable at a time.
    // Stack counts from -1
    if (stack_temp_vars->top != 0) {
        fprintf(stderr, "Invalid number of values for a define. Only 1 is allowed, got %d\n", stack_temp_vars->top + 1);
        parser_end(ERROR_SEM);
    }

    // Skip expression if on the left side of the define is an underscore.
    if (strcmp(id, "_") == 0) {
        stack_charptr_pop(stack_temp_vars);
        return;
    }

    char *res = (char *)stack_charptr_pop(stack_temp_vars);
    symtable_symbol_t *res_symbol = symtable_find_symbol(st, res);

    // Set type of the newly defined variable
    symbol->token.type = res_symbol->token.type;
    switch (res_symbol->token.type) {
    case INT_LIT:
        symbol->token.attribute.int_val = res_symbol->token.attribute.int_val;
        break;
    case FLOAT64_LIT:
        symbol->token.attribute.float_val = res_symbol->token.attribute.float_val;
        break;
    default:
        break;
    }

    res = TAC_get_identifier(res);
    char *ident = TAC_get_identifier(id);

    TAC_insert(tac_list, OP_MOVE, res, NULL, ident);
}

// my_list needed for the for statement
void parser_assign(char *id, TACList *my_list) {
    debug_entry();

    if (strcmp(id, "_") != 0 && symtable_find_symbol(st, id) == NULL) {
        fprintf(stderr, "Variable %s is not defined\n", id);
        parser_end(ERROR_SEM_VAR);
    }

    debug("Push id \"%s\" on stack", id);
    stack_charptr_push(stack_assigns, id);

    parser_id_n();
    // Reverse stack of assigns, because the last added value is currently the
    // first available. We need it to be the other way around.
    stack_charptr_reverse(stack_assigns);
    int num_of_assigns = stack_assigns->top + 1;
    max_func_returns = num_of_assigns;

    parser_match(ASSIGN);
    parser_exprs();

    // Also reverse the stack of temporary variables to resolve the assigns
    // from left to right
    stack_charptr_reverse(stack_temp_vars);

    debug("ASS: %d, TMP: %d",stack_assigns->top, stack_temp_vars->top);
    if (stack_assigns->top != stack_temp_vars->top) {
        fprintf(stderr, "Mismatched number of assigned values\n");
        parser_end(ERROR_SEM_PROGRAM);
    }

    while (num_of_assigns > 0) {
        char *res_ident = (char *) stack_charptr_pop(stack_temp_vars);
        symtable_symbol_t *res = symtable_find_symbol(st, res_ident);

        char *assign_ident = (char *) stack_charptr_pop(stack_assigns);
        symtable_symbol_t *assign = symtable_find_symbol(st, assign_ident);

        debug("SPIN");
        if (strcmp(assign_ident, "_") == 0) {
            debug("SKIP");
            num_of_assigns--;
            continue;
        }

        if (res->token.type != assign->token.type) {
            fprintf(stderr, "Invalid assigned value. Expected %s, got %s\n", token_get_type_string(assign->token.type), token_get_type_string(res->token.type));
            parser_end(ERROR_SEM_COMPATIBILITY);
        }

        switch (assign->token.type) {
        case INT_LIT:
            assign->token.attribute.int_val = res->token.attribute.int_val;
            break;
        case FLOAT64_LIT:
            assign->token.attribute.float_val = res->token.attribute.float_val;
            break;
        default:
            break;
        }

        TAC_insert(my_list, OP_MOVE, TAC_get_identifier(res_ident), NULL, TAC_get_identifier(assign_ident));

        num_of_assigns--;
    }
}

void parser_id_n() {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        char *id = lookahead.attribute.sym_key;
        if (strcmp(id, "_") != 0 && symtable_find_symbol(st, id) == NULL) {
            fprintf(stderr, "Variable %s is not defined\n", id);
            parser_end(ERROR_SEM_VAR);
        }

        debug("Push id \"%s\" on stack", id);
        stack_charptr_push(stack_assigns, id);

        parser_match(IDENT);
        parser_id_n();
    }
    // apply eps-rule
}

void parser_exprs() {
    debug_entry();

    /*
    const char* target_id = NULL;
    token_type target_type = INVALID;

    if (!stack_charptr_isempty(stack_assigns)) {
        target_id = stack_charptr_pop(stack_assigns);
        if (strcmp(target_id, "_") == 0)
            target_type = INVALID;
        else
            target_type = symtable_find_symbol(st, target_id)->type;
        debug("Variable \"%s\" holds value of type \"%s\"", target_id,  token_get_type_string(target_type));
    }
    */

    parser_expr();

    parser_expr_n();
}

void parser_expr_n() {
    debug_entry();
    // if COMMA not found, apply eps-rule
    if (lookahead.type == COMMA) {
        parser_match(COMMA);

        /*
        const char* target_id = NULL;
        token_type target_type = INVALID;

        if (!stack_charptr_isempty(stack_assigns)) {
            target_id = stack_charptr_pop(stack_assigns);
            target_type = symtable_find_symbol(st, target_id)->type;
        }
        */

        parser_expr();
        parser_expr_n();
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

// my_list needed for for statement
void parser_optassign(TACList *my_list) {
    debug_entry();
    // if IDENT not found, apply eps-rule
    if (lookahead.type == IDENT) {
        char *id = lookahead.attribute.sym_key;
        parser_match(IDENT);

        switch (lookahead.type) {
            case ASSIGN:
            case COMMA:
                // variable assign
                parser_assign(id, my_list);
                required_eol = false;  // in for loop -> no need for eol
                break;
            case ADD_ASSIGN:
                // += assign
                parser_add_assign(id, my_list);
                required_eol = false;  // in for loop -> no need for eol
                break;
            case SUB_ASSIGN:
                // -= assign
                parser_sub_assign(id, my_list);
                required_eol = false;  // in for loop -> no need for eol
                break;
            case MUL_ASSIGN:
                // *= assign
                parser_mul_assign(id, my_list);
                required_eol = false;  // in for loop -> no need for eol
                break;
            case DIV_ASSIGN:
                // /= assign
                parser_div_assign(id, my_list);
                required_eol = false;  // in for loop -> no need for eol
                break;
            default:
                // no valid operator was matched -> syntax error
                parser_end(ERROR_SYNTAX);
                break;
        }
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

    parser_optexprs();

    if (num_of_returns != stack_temp_vars->top + 1) {
        fprintf(stderr, "Wrong number of returned values\n");
        parser_end(ERROR_SEM_PROGRAM);
    }

    if (num_of_returns > 0)
        stack_charptr_reverse(stack_temp_vars);

    int return_num = 1;
    func_return_t *def_return = symtable_get_func_return(st);
    while (num_of_returns != 0) {
        char *res_ident = (char *) stack_charptr_pop(stack_temp_vars);
        symtable_symbol_t *res = symtable_find_symbol(st, res_ident);

        if (def_return->type != res->token.type) {
            fprintf(stderr, "Mismatched return. Expected %s, got %s\n", token_get_type_string(def_return->type), token_get_type_string(res->token.type));
            parser_end(ERROR_SEM_PROGRAM);
        }

        char *ret_ident = TAC_create_return_value(return_num);
        TAC_insert(tac_list, OP_DEFINE, NULL, NULL, ret_ident);
        TAC_insert(tac_list, OP_MOVE, TAC_get_identifier(res_ident), NULL, ret_ident);

        def_return = def_return->next_return;
        return_num++;
        num_of_returns--;
    }

    TAC_insert(tac_list, OP_RETURN, NULL, NULL, NULL);
}

void parser_optexprs() {
    debug_entry();
    // optional expression depending on the type read
    switch (lookahead.type) {
        case LPAREN:
        case INT_LIT:
        case FLOAT64_LIT:
        case STRING_LIT:
        case IDENT:
            parser_exprs();
            break;
        default:
            // apply eps-rule
            break;
    }
}

/* ------------------------------------------------------------------------ */
/* EXPRESSION RULES - BOTTOM-UP PRECEDENCE ANALYSIS                         */
/* ------------------------------------------------------------------------ */

// Define some STACK functions needed specifically for the precedence analysis.

token_t prec_create_expr_symbol() {
    token_t expr = { .type = EXPR_SYMBOL };
    return expr;
}

token_t prec_create_shift_symbol() {
    token_t shift = { .type = SHF };
    return shift;
}

token_t prec_create_EOI_symbol() {
    token_t end_of_input = { .type = END_OF_INPUT };
    return end_of_input;
}

bool stack_top_nonterminal(stack_token_t *stack, token_t *token) {
    debug_entry();
    int top = stack->top;

    while (top >= 0) {
        if (stack->array[top].type != EXPR_SYMBOL && stack->array[top].type != SHF) {
            *token = stack->array[top];
            return true;
        }
        top--;
    }

    return false;
}

void stack_shift(stack_token_t *stack, token_t terminal) {
    debug_entry();
    // resize the stack if the stack is full, we need more space
    if (stack_token_t_isfull(stack)) {
        stack_token_t_resize(stack);
    }

    int terminal_pos = stack_token_t_find(stack, terminal, cmp_token_t);
    int curr = terminal_pos + 1;
    stack->top++;  // increment the top value to compensate for appended symbol

    // rearrange everything that has beDEEPERen located before the terminal
    while (curr != stack->top) {
        token_t old = stack->array[curr];
        curr++;
        stack->array[curr] = old;
    }

    // append the shift symbol
    stack->array[terminal_pos + 1] = prec_create_shift_symbol();
}

bool stack_reduce(stack_token_t *stack, stack_int_t *function_stack) {
    debug_entry();
    // if '<y' is on top of the stack and rule exists
    token_t top = stack_token_t_peek(stack);

    // get the positions of the top symbol and the shift symbol
    int top_pos = stack_token_t_find(stack, top, cmp_token_t);
    token_t shift = prec_create_shift_symbol();
    int shift_pos = stack_token_t_find(stack, shift, cmp_token_t);

    if (shift_pos == -1 || top_pos == -1 || shift_pos > top_pos) {
        return false;
    }

    stack_token_t *rule_stack = stack_token_t_init();
    // determine if any other symbols are placed between these two
    for (int i = shift_pos + 1; i <= top_pos; i++) {
        token_t token = stack_token_t_at(stack, i);
        stack_token_t_push(rule_stack, token);
    }

    // check the rules
    bool success = prec_apply_rule(rule_stack, function_stack);

    stack_token_t_free(rule_stack);
    if (!success) {
        return false;
    }

    // actually change the symbols to E
    stack->top = shift_pos;
    stack->array[stack->top] = prec_create_expr_symbol();

    return true;
}

// Precedence table definition.
const token_type PREC_TABLE[PREC_TABLE_OP_COUNT][PREC_TABLE_OP_COUNT] =
{      //   ,   REL  ADD  MUL   (    )   id   lit   $
/*  ,  */ {EQQ, SHF, SHF, SHF, SHF, EQQ, SHF, SHF, INV},
/* REL */ {RED, RED, SHF, SHF, SHF, RED, SHF, SHF, RED},
/* ADD */ {RED, RED, UNA, SHF, SHF, RED, SHF, SHF, RED},
/* MUL */ {RED, RED, RED, RED, SHF, RED, SHF, SHF, RED},
/*  (  */ {EQQ, SHF, SHF, SHF, SHF, EQQ, SHF, SHF, INV},
/*  )  */ {RED, RED, RED, RED, INV, RED, INV, INV, RED},
/* id  */ {RED, RED, RED, RED, EQQ, RED, INV, INV, RED},
/* lit */ {RED, RED, RED, RED, INV, RED, INV, INV, RED},
/*  $  */ {SHF, SHF, SHF, SHF, SHF, INV, SHF, SHF, INV}
};

int prec_get_table_index(token_type t) {
    if (t == COMMA) {
        return 0;
    } else if (is_relop(t)) {
        return 1;
    } else if (is_addop(t)) {
        return 2;
    } else if (is_mulop(t)) {
        return 3;
    } else if (t == LPAREN) {
        return 4;
    } else if (t == RPAREN) {
        return 5;
    } else if (t == IDENT) {
        return 6;
    } else if (token_is_lit(t)) {
        return 7;
    } else if (END_OF_INPUT) {
        return 8;
    } else {
        // will probably never throw
        throw_syntax_error(t, line);
    }
}

/**
 * --- basic ---
 * E : id
 * E : lit
 * E : ( E )
 * E : E REL E
 * E : E ADD E
 * E : E MUL E
 * --- FUNC  ---
 * E : id ( )
 * E : id ( E )
 * E : id ( E , ... )
 * --- UNARY ---
 * E : ADD E
 */
bool prec_apply_rule(stack_token_t *rule_stack, stack_int_t *function_stack) {
    debug_entry();

    token_t curr_token;      // current token
    token_type curr_symbol;  // current symbol
    NEXT_SYMBOL;

    if (expr_has_sus_func_call) {
        debug("Sus function call");
        func_call_t fc = stack_func_call_t_peek(stack_func_calls);
        if (fc.num_of_expected_returns != 1) {
            fprintf(stderr, "Function call cannot return more than 1 value in an expression\n");
            parser_end(ERROR_SEM_PROGRAM);
        }

        if (fc.first_assign->type == INVALID) {
            debug("Function is sus for a reason");
            int i = rule_stack->top;
            while (i >= 0) {
                token_t rule_token = rule_stack->array[i];

                if (token_is_lit(rule_token.type)) {
                    debug("Using literal value: %s", token_get_type_string(rule_token.type));
                    fc.first_assign->type = rule_token.type;
                    break;
                }

                if (rule_token.type == IDENT) {
                     // This is not 100% ok because an identiier can also have INVALID type.
                    // Oh well.
                    char *rule_ident = rule_token.attribute.sym_key;
                    symtable_symbol_t *rule = symtable_find_symbol(st, rule_ident);
                    debug("Using type in a variable: %s", token_get_type_string(rule->token.type));

                    fc.first_assign->type = rule->token.type;
                    break;
                }
                i--;
            }
            if (fc.first_assign->type == INVALID) {
                debug("Could not resolve");
            }
        }

        expr_has_sus_func_call = false;
    }

    // switch to determine the rightmost symbol
    switch (curr_symbol) {
        case IDENT: {
                // E : id
                debug("IDENT");

                char *source_ident = curr_token.attribute.sym_key;
                symtable_symbol_t *source = symtable_find_symbol(st, source_ident);
                if (source == NULL) {
                    fprintf(stderr, "Undefined variable %s\n", source_ident);
                    parser_end(ERROR_SEM_VAR);
                }
                char *target_ident = create_temporary_variable(source->token.type);
                symtable_symbol_t *target = symtable_find_symbol(st, target_ident);

                // Set type for values from function calls (only set if not set already)
                if (stack_int_peek(stack_func_ret_types) == INVALID) {
                    stack_int_pop(stack_func_ret_types);
                    stack_int_push(stack_func_ret_types, source->token.type);
                }

                switch (source->token.type) {
                case INT_LIT:
                    target->token.attribute.int_val = source->token.attribute.int_val;
                    break;
                case FLOAT64_LIT:
                    target->token.attribute.float_val = source->token.attribute.float_val;
                    break;
                case STRING_LIT:
                    target->token.attribute.str_val = source->token.attribute.str_val;
                    break;
                default:
                    fprintf(stderr, "Encountered unhandled literal: %s\n", token_get_type_string(source->token.type));
                    parser_end(ERROR_SEM);
                    break;
                }

                TAC_insert(tac_list, OP_MOVE, TAC_get_identifier(source_ident), NULL, TAC_get_identifier(target_ident));
            }
            break;
        case INT_LIT:
        case FLOAT64_LIT:
        case STRING_LIT: {
                // E : lit
                debug("LITERAL");
                char *target_ident = create_temporary_variable(curr_symbol);
                symtable_symbol_t *target = symtable_find_symbol(st, target_ident);

                // Set type for values from function calls (only set if not set already)
                if (stack_int_peek(stack_func_ret_types) == INVALID) {
                    stack_int_pop(stack_func_ret_types);
                    stack_int_push(stack_func_ret_types, curr_symbol);
                }

                char *arg = NULL;
                switch (curr_symbol) {
                case INT_LIT:
                    arg = TAC_get_int(curr_token.attribute.int_val);
                    target->token.attribute.int_val = curr_token.attribute.int_val;
                    break;
                case FLOAT64_LIT:
                    arg = TAC_get_float(curr_token.attribute.float_val);
                    target->token.attribute.float_val = curr_token.attribute.float_val;
                    break;
                case STRING_LIT:
                    arg = TAC_get_string(curr_token.attribute.str_val);
                    target->token.attribute.str_val = curr_token.attribute.str_val;
                    break;
                default:
                    fprintf(stderr, "Unhandled literal encountered\n");
                    parser_end(ERROR_SEM);
                    break;
                }

                TAC_insert(tac_list, OP_MOVE, arg, NULL, TAC_get_identifier(target_ident));
            }
            break;
        case RPAREN:
            // E : ( E )
            // E : id ( )
            // E : id ( E )
            // E : id ( ... , E )
            return prec_rule_rparen(rule_stack, function_stack);
        case EXPR_SYMBOL: {
                // E : ADD E
                NEXT_SYMBOL;
                debug("EXPR_SYMBOL");
                if (is_addop(curr_symbol) && stack_token_t_isempty(rule_stack)) {
                    // Unary
                    debug("UNARY");

                    const char *operand_ident = stack_charptr_pop(stack_temp_vars);
                    symtable_symbol_t *operand = symtable_find_symbol(st, operand_ident);

                    // Set type for values from function calls (only set if not set already)
                    if (stack_int_peek(stack_func_ret_types) == INVALID) {
                        stack_int_pop(stack_func_ret_types);
                        stack_int_push(stack_func_ret_types, curr_symbol);
                    }

                    if (operand->token.type == STRING_LIT) {
                        fprintf(stderr, "Unary operator can't be used with a string\n");
                        parser_end(ERROR_SEM_COMPATIBILITY);
                    }

                    if (curr_symbol == SUB) {
                        char *target_ident = create_temporary_variable(operand->token.type);
                        symtable_symbol_t *target = symtable_find_symbol(st, target_ident);

                        operator_type tac_operation = OP_INVALID;
                        if (operand->token.type == INT_LIT) {
                            target->token.attribute.int_val = -operand->token.attribute.int_val;
                            // tac_operation = OP_UNARY_SUB_INT;
                        }
                        else {
                            target->token.attribute.float_val = -operand->token.attribute.float_val;
                            // tac_operatio = OP_UNARY_SUB_FLOAT;
                        }

                        TAC_insert(tac_list, tac_operation, TAC_get_identifier((char *) operand_ident), NULL, TAC_get_identifier(target_ident));
                        operand_ident = stack_charptr_pop(stack_temp_vars);
                    }

                    char *target_ident = create_temporary_variable(operand->token.type);
                    // TODO: Properly prepare argument
                    TAC_insert(tac_list, OP_MOVE, (char *) operand_ident, NULL, TAC_get_identifier(target_ident));
                    break;
                }

                if (!is_relop(curr_symbol) && !is_addop(curr_symbol) && !is_mulop(curr_symbol)) {
                    fprintf(stderr, "Encountered incorrect mathematic operation\n");
                    return false;
                }

                // E : E REL E
                // E : E ADD E
                // E : E MUL E
                debug("CLASSIC OPERATION");
                token_type sem_operation = curr_symbol;

                NEXT_SYMBOL;
                if (curr_symbol != EXPR_SYMBOL) {
                    fprintf(stderr, "Received invalid operand for math operation\n");
                    return false;
                }

                // Prepare both operands (the right one was processed later -> is on the top of the
                // stack
                char *right_operand_ident = (char *) stack_charptr_pop(stack_temp_vars);
                char *left_operand_ident = (char *) stack_charptr_pop(stack_temp_vars);

                symtable_symbol_t *right_operand = symtable_find_symbol(st, right_operand_ident);
                symtable_symbol_t *left_operand = symtable_find_symbol(st, left_operand_ident);

                if (is_relop(sem_operation)) {
                    if (left_operand->token.type == BOOL_LIT) {
                        left_operand->token.type = right_operand->token.type;
                    }
                    if (right_operand->token.type == BOOL_LIT) {
                        right_operand->token.type = left_operand->token.type;
                    }
                }

                if (left_operand->token.type != right_operand->token.type) {
                    fprintf(stderr, "Expression operands are not of the same type. Left is %s, right is %s\n",
                            token_get_type_string(left_operand->token.type),
                            token_get_type_string(right_operand->token.type));
                    parser_end(ERROR_SEM_COMPATIBILITY);
                }

                if (stack_int_peek(stack_func_ret_types) == INVALID) {
                    stack_int_pop(stack_func_ret_types);
                    stack_int_push(stack_func_ret_types, left_operand->token.type);
                }

                operator_type tac_operation = OP_INVALID;
                switch (sem_operation) {
                case ADD:
                    if (left_operand->token.type == STRING_LIT)
                        tac_operation = OP_ADD_STRING;
                    else
                        tac_operation = OP_ADD;
                    break;
                case SUB:
                    tac_operation = OP_SUB;
                    break;
                case MUL:
                    tac_operation = OP_MUL;
                    break;
                case DIV:
                    if (left_operand->token.type == INT_LIT)
                        tac_operation = OP_IDIV;
                    else
                        tac_operation = OP_DIV;
                    break;
                case AND:
                    tac_operation = OP_AND;
                    break;
                case OR:
                    tac_operation = OP_OR;
                    break;
                case EQL:
                    tac_operation = OP_EQL;
                    break;
                case NEQ:
                    tac_operation = OP_NEQ;
                    break;
                case LSS:
                    tac_operation = OP_LSS;
                    break;
                case LEQ:
                    tac_operation = OP_LEQ;
                    break;
                case GTR:
                    tac_operation = OP_GTR;
                    break;
                case GEQ:
                    tac_operation = OP_GEQ;
                    break;
                default:
                    fprintf(stderr, "Unhandled mathematic operation\n");
                    parser_end(ERROR_SEM);
                    break;
                }

                token_type target_type = INVALID;
                if (is_relop(sem_operation)) {
                    target_type = BOOL_LIT;
                } else {
                    target_type = left_operand->token.type;
                }
                char *target_ident = create_temporary_variable(target_type);
                symtable_symbol_t *target = symtable_find_symbol(st, target_ident);

                // Execute operation on the semantics level (only for operations on numbers)
                if (target->token.type == INT_LIT) {
                    switch (sem_operation) {
                    case ADD:
                        target->token.attribute.int_val = left_operand->token.attribute.int_val + right_operand->token.attribute.int_val;
                        break;
                    case SUB:
                        target->token.attribute.int_val = left_operand->token.attribute.int_val - right_operand->token.attribute.int_val;
                        break;
                    case MUL:
                        target->token.attribute.int_val = left_operand->token.attribute.int_val * right_operand->token.attribute.int_val;
                        break;
                    case DIV:
                        if (right_operand->token.attribute.int_val == 0) {
                            fprintf(stderr, "Can't divide by zero\n");
                            parser_end(ERROR_ZERO);
                        }
                        target->token.attribute.int_val = left_operand->token.attribute.int_val / right_operand->token.attribute.int_val;
                        break;
                    default:
                        break;
                    }
                } else if (target->token.type == FLOAT64_LIT) {
                    switch (sem_operation) {
                    case ADD:
                        target->token.attribute.float_val = left_operand->token.attribute.float_val + right_operand->token.attribute.float_val;
                        break;
                    case SUB:
                        target->token.attribute.float_val = left_operand->token.attribute.float_val - right_operand->token.attribute.float_val;
                        break;
                    case MUL:
                        target->token.attribute.float_val = left_operand->token.attribute.float_val * right_operand->token.attribute.float_val;
                        break;
                    case DIV:
                    if (right_operand->token.attribute.float_val == 0) {
                            fprintf(stderr, "Can't divide by zero\n");
                            parser_end(ERROR_ZERO);
                        }
                        target->token.attribute.float_val = left_operand->token.attribute.float_val / right_operand->token.attribute.float_val;
                        break;
                    default:
                        break;
                    }
                }

                TAC_insert(tac_list, tac_operation,
                        TAC_get_identifier(left_operand_ident),
                        TAC_get_identifier(right_operand_ident),
                        TAC_get_identifier(target_ident));
            }
            break;
        default:
            fprintf(stderr, "Unexpected symbol while applying rule\n");
            return false;
    }

    return true;
}

bool prec_rule_rparen(stack_token_t *rule_stack, stack_int_t *function_stack) {
    debug_entry();
    token_t curr_token;      // current token
    token_type curr_symbol;  // current symbol

    // go to the next symbol
    NEXT_SYMBOL;

    // test the symbol
    if (curr_symbol != EXPR_SYMBOL && curr_symbol != LPAREN) {
        fprintf(stderr, "Expected an expression or a parenthesis\n");
        return false;
    }

    // Fact: A function returns at least one value
    // Fact: Multiple returned values are allowed only in assigns
    if (curr_symbol == LPAREN) {
        // E : id ( )
        if (stack_func_ret_types->top >= 0) {
            stack_int_pop(stack_func_ret_types);
        }

        NEXT_SYMBOL;
        if (curr_symbol != IDENT) {
            fprintf(stderr, "Expected function call with no parameters\n");
            return false;
        }

        // TODO: Check if a variable with the same id exists in the same scope

        // Track function call for later evaluation
        func_call_t fc;
        func_call_init(&fc);
        func_set_id(&fc.func, curr_token.attribute.sym_key);

        TAC_insert(tac_list, OP_CREATE_FRAME, NULL, NULL, NULL);
        TAC_insert(tac_list, OP_CALL, curr_token.attribute.sym_key, NULL, NULL);

        // Handle returned values
        if (stack_int_peek(stack_func_ret_types) != INVALID || function_stack->top > 0) {
            debug("Only 1");
            token_type func_ret_type = stack_int_peek(stack_func_ret_types);
            func_call_add_assign(&fc, func_ret_type);
            
            char *target_ident = create_temporary_variable(func_ret_type);

            // Returns in generated code are numbered from 1
            char *ret = TAC_create_return_value(1);
            TAC_insert(tac_list, OP_MOVE, ret, NULL, TAC_get_identifier(target_ident));
        } else {
            debug("Follow assigns: %d", max_func_returns);
            for (int ret_num = 0; ret_num < max_func_returns; ret_num++) {
                char *assign_ident = (char *) stack_assigns->array[stack_assigns->top - ret_num];
                symtable_symbol_t *assign = symtable_find_symbol(st, assign_ident);

                token_type assign_type;
                // assign will be NULL if on the left side is "identifier" _
                if (assign == NULL)
                    assign_type = INVALID;
                else
                    assign_type = assign->token.type;

                func_call_add_assign(&fc, assign_type);

                char *target_ident = create_temporary_variable(assign_type);

                // Returns in generated code are numbered from 1
                char *ret = TAC_create_return_value(ret_num+1);
                TAC_insert(tac_list, OP_MOVE, ret, NULL, TAC_get_identifier(target_ident));
            }
        }
        stack_func_call_t_push(stack_func_calls, fc);

        expr_has_sus_func_call = true;
        stack_int_pop(function_stack);
        stack_int_pop(stack_func_ret_types);
        return true;
    }

    func_call_t fc;
    func_call_init(&fc);

    // curr_symbol == E
    char *arg_ident = (char *) stack_charptr_pop(stack_temp_vars);
    if (arg_ident == NULL) {
        fprintf(stderr, "Expected to get argument identifier\n");
        parser_end(ERROR_SEM);
    }
    symtable_symbol_t *arg_symbol = symtable_find_symbol(st, arg_ident);
    if (arg_symbol == NULL) {
        fprintf(stderr, "Expected to get a symbol of an argument\n");
        parser_end(ERROR_SEM);
    }
    func_add_parameter(&fc.func, arg_ident, arg_symbol->token.type);
    debug("Potential func call param: %s", token_get_type_string(arg_symbol->token.type));

    NEXT_SYMBOL;
    if (curr_symbol != LPAREN && curr_symbol != COMMA) {
        return false;
    }

    if (curr_symbol == LPAREN) {
        if (stack_token_t_isempty(rule_stack)) {
            // E : ( E )
            // The info about the symbol is in the prepared "function call" structure
            char *target_ident = create_temporary_variable(arg_symbol->token.type);
            symtable_symbol_t *target = symtable_find_symbol(st, target_ident);

            target->token.type = arg_symbol->token.type;
            switch (target->token.type) {
            case INT_LIT:
                target->token.attribute.int_val = arg_symbol->token.attribute.float_val;
                break;
            case FLOAT64_LIT:
                target->token.attribute.float_val = arg_symbol->token.attribute.float_val;
                break;
            default:
                break;
            }

            TAC_insert(tac_list, OP_MOVE, TAC_get_identifier(arg_ident), NULL, TAC_get_identifier(target_ident));
            return true;
        } else {
            // E : id ( E )
            // Track function call for later evaluation
            if (stack_func_ret_types->top >= 0) {
                stack_int_pop(stack_func_ret_types);
            }

            NEXT_SYMBOL;
            if (curr_symbol != IDENT) {
                return false;
            }

            TAC_insert(tac_list, OP_CREATE_FRAME, NULL, NULL, NULL);

            // TODO: Check if a variable with the same id exists in the same scope
            char *arg = TAC_create_function_argument(1);
            TAC_insert(tac_list, OP_DEFINE, NULL, NULL, arg);
            TAC_insert(tac_list, OP_MOVE, TAC_get_identifier((char *) arg_ident), NULL, arg);

            func_set_id(&fc.func, curr_token.attribute.sym_key);

            TAC_insert(tac_list, OP_CALL, curr_token.attribute.sym_key, NULL, NULL);

            // Handle returned values
            debug("FUNC RET %s, FUNC STACK: %d", token_get_type_string(stack_int_peek(stack_func_ret_types)), function_stack->top);
            if (stack_int_peek(stack_func_ret_types) != INVALID || function_stack->top > 0) {
                debug("Only 1");
                token_type func_ret_type = stack_int_peek(stack_func_ret_types);
                func_call_add_assign(&fc, func_ret_type);
                
                char *target_ident = create_temporary_variable(func_ret_type);

                // Returns in generated code are numbered from 1
                char *ret = TAC_create_return_value(1);
                TAC_insert(tac_list, OP_MOVE, ret, NULL, TAC_get_identifier(target_ident));
            } else {
                for (int ret_num = 0; ret_num < max_func_returns; ret_num++) {
                    debug("FOLLOW ASSIGNS");
                    const char *assign_ident = stack_assigns->array[stack_assigns->top - ret_num];
                    symtable_symbol_t *assign = symtable_find_symbol(st, assign_ident);

                    token_type assign_type;
                    // assign will be NULL if on the left side is "identifier" _
                    if (assign == NULL)
                        assign_type = INVALID;
                    else
                        assign_type = assign->token.type;

                    func_call_add_assign(&fc, assign_type);

                    char *target_ident = create_temporary_variable(assign_type);

                    // Returns in generated code are numbered from 1
                    char *ret = TAC_create_return_value(ret_num+1);
                    TAC_insert(tac_list, OP_MOVE, ret, NULL, TAC_get_identifier(target_ident));
                }
            }

            stack_func_call_t_push(stack_func_calls, fc);

            expr_has_sus_func_call = true;
            stack_int_pop(function_stack);
            stack_int_pop(stack_func_ret_types);
            return true;
        }
    }

    if (stack_func_ret_types->top >= 0) {
        stack_int_pop(stack_func_ret_types);
    }

    // curr_symbol == ,
    // E : id ( ... , E )
    while (!stack_token_t_isempty(rule_stack)) {
        NEXT_SYMBOL;
        if (curr_symbol != EXPR_SYMBOL) {
            fprintf(stderr, "Expected to get an expression as an argument\n");
            return false;
        }

        char *arg_ident = (char *) stack_charptr_pop(stack_temp_vars);
        if (arg_ident == NULL) {
            fprintf(stderr, "Expected to get argument identifier\n");
            parser_end(ERROR_SEM);
        }
        symtable_symbol_t *arg_symbol = symtable_find_symbol(st, arg_ident);
        if (arg_symbol == NULL) {
            fprintf(stderr, "Expected to get a symbol of an argument\n");
            parser_end(ERROR_SEM);
        }
        debug("Func call param: %s", token_get_type_string(arg_symbol->token.type));
        func_add_parameter(&fc.func, arg_ident, arg_symbol->token.type);

        NEXT_SYMBOL;
        if (curr_symbol != COMMA) {
            break;
        }
    }

    TAC_insert(tac_list, OP_CREATE_FRAME, NULL, NULL, NULL);

    // Beware, the arguments need to be called from left to right (pascal convention)
    // The list of arguments has to be reversed here to get the order right
    func_parameter_t *last_param = fc.func.first_parameter;
    while (last_param->next_param != NULL) {
        last_param = last_param->next_param;
    }
    fc.func.first_parameter = last_param;
    last_param = last_param->prev_param;
    func_parameter_t *first_param = fc.func.first_parameter;
    
    unsigned arg_num = 1;
    while (first_param != NULL) {
        first_param->next_param = last_param;
        char *arg = TAC_create_function_argument(arg_num);
        
        TAC_insert(tac_list, OP_DEFINE, NULL, NULL, arg);
        TAC_insert(tac_list, OP_MOVE, TAC_get_identifier(first_param->id.str), NULL, arg);

        arg_num++;
        debug("1");
        if (last_param != NULL)
            last_param = last_param->prev_param;
        first_param = first_param->next_param;
        debug("2");
    }

    if (curr_symbol != LPAREN) {
        fprintf(stderr, "Expected to get an opening parenthesis\n");
        return false;
    }

    NEXT_SYMBOL;
    if (curr_symbol != IDENT) {
        fprintf(stderr, "Expected to get function identifier");
        return false;
    }

    func_set_id(&fc.func, curr_token.attribute.sym_key);
    // TODO: Check if a variable with the same id exists in the same scope

    TAC_insert(tac_list, OP_CALL, curr_token.attribute.sym_key, NULL, NULL);

    // Handle returned values
    if (stack_int_peek(stack_func_ret_types) != INVALID || function_stack->top > 0) {
        token_type func_ret_type = stack_int_peek(stack_func_ret_types);
        debug("Only 1: %s", token_get_type_string(func_ret_type));
        func_call_add_assign(&fc, func_ret_type);
        
        char *target_ident = create_temporary_variable(func_ret_type);

        // Returns in generated code are numbered from 1
        char *ret = TAC_create_return_value(1);
        TAC_insert(tac_list, OP_MOVE, ret, NULL, TAC_get_identifier(target_ident));
    } else {
        for (int ret_num = 0; ret_num < max_func_returns; ret_num++) {
            const char *assign_ident = stack_assigns->array[stack_assigns->top - ret_num];
            symtable_symbol_t *assign = symtable_find_symbol(st, assign_ident);

            token_type assign_type;
            // assign will be NULL if on the left side is "identifier" _
            if (assign == NULL)
                assign_type = INVALID;
            else
                assign_type = assign->token.type;

            func_call_add_assign(&fc, assign_type);

            char *target_ident = create_temporary_variable(assign_type);

            // Returns in generated code are numbered from 1
            char *ret = TAC_create_return_value(ret_num+1);
            TAC_insert(tac_list, OP_MOVE, ret, NULL, TAC_get_identifier(target_ident));
        }
    }
    stack_func_call_t_push(stack_func_calls, fc);

    expr_has_sus_func_call = true;
    stack_int_pop(function_stack);
    stack_int_pop(stack_func_ret_types);
    return true;
}

void prec_clean(stack_token_t *token_stack, stack_int_t *func_stack) {
    stack_token_t_free(token_stack);
    stack_int_free(func_stack);
    // stack_int_free(stack_func_ret_types);
}

void parser_precedence_analysis() {
    debug_entry();

    stack_token_t *token_stack = stack_token_t_init();
    if (token_stack == NULL) {
        parser_end(ERROR_INTERNAL);
    }
    stack_int_t *function_stack = stack_int_init();
    if (function_stack == NULL) {
        parser_end(ERROR_INTERNAL);
    }

    stack_func_ret_types = stack_int_init();
    if (stack_func_ret_types == NULL) {
        parser_end(ERROR_INTERNAL);
    }

    stack_int_push(stack_func_ret_types, INVALID);
    expr_has_sus_func_call = false;

    // put $ at the top of the stack
    token_t end_of_input = prec_create_EOI_symbol();
    stack_token_t_push(token_stack, end_of_input);

    token_t input = lookahead;
    bool stop = false;

    // start the precedence algorithm
    while (true) {
        debug("got eol? %d no_eol? %d required_eol? %d", eol_encountered, no_eol, required_eol);
        debug("succesful stack? %d stop? %d\n", prec_succesful_stack(token_stack), stop);

        // break the loop if conditions are met
        if (prec_succesful_stack(token_stack) && eol_encountered) {
            break;
        } else if (stop && input.type == END_OF_INPUT) {
            break;
        } else if (stop) {
            input = prec_create_EOI_symbol();
        }

        // check if the identifier is '_' -> semantic error
        if (input.type == IDENT && strcmp(input.attribute.sym_key, "_") == 0) {
           prec_clean(token_stack, function_stack);
           parser_end(ERROR_SEM_VAR);
        }

        // handle the symbol accordingly
        if (prec_handle_symbol(token_stack, input, &stop, function_stack)) {
            parser_move();
            input = lookahead;
        }

        // do some things depending on the prec_handle_symbol function

        // Mutiple assign statement, consider the comma as the end of the input
        if ((stack_int_isempty(function_stack) && input.type == COMMA)) {
            input = prec_create_EOI_symbol();
        }
        // FIXME: remove this if not needed
        /*if ((stack_int_isempty(function_stack) && input.type == COMMA) ||*/
                /*(stack_int_isempty(function_stack) && input.type == LBRACE)) {*/
            /*input = prec_create_EOI_symbol();*/
        /*}*/

        if (eol_encountered) {
            token_t top_token = stack_token_t_peek(token_stack);
            token_type top = top_token.type;
            token_type t = input.type;

            // eol encountered, is this the end of the input?
            if (token_is_lit(top) || top == IDENT || top == RPAREN) {
                // top symbol is literal, id or ')'
                if (is_addop(t) || is_mulop(t) || is_relop(t) ||
                        t == COMMA || t == RPAREN) {
                    // if the following character is some operator, we can't break line
                    prec_clean(token_stack, function_stack);
                    parser_end(ERROR_SYNTAX);
                } else {
                    // otherwise, it should be the end of the input
                    input = prec_create_EOI_symbol();
                }
            }
        }
    }

    bool success = prec_succesful_stack(token_stack);
    prec_clean(token_stack, function_stack);

    if (!success) {
        parser_end(ERROR_SYNTAX);
    }
}

bool prec_succesful_stack(stack_token_t *token_stack) {
    return token_stack->top == 1 &&
           token_stack->array[1].type == EXPR_SYMBOL &&
           token_stack->array[0].type == END_OF_INPUT;
}

bool prec_handle_symbol(stack_token_t *token_stack, token_t input, bool *stop, stack_int_t *function_stack) {
    // TODO: Should we check the input if it makes sense and throw syntax error right here?

    // terminal on top of the stack
    token_t top;
    if (!stack_top_nonterminal(token_stack, &top)) {
        // no terminal on top - weird, probably isn't needed
        prec_clean(token_stack, function_stack);
        parser_end(ERROR_SYNTAX);
    }

    int row = prec_get_table_index(top.type);
    int col = prec_get_table_index(input.type);
    debug("Top T: %s, Top: %s, input: %s",
            token_get_type_string(top.type),
            token_get_type_string(stack_token_t_peek(token_stack).type),
            token_get_type_string(input.type));

    switch(PREC_TABLE[row][col]) {
        case UNA:
            // '>|<' unary symbol
            // check what is in stack and jump to RED or SHF
            debug("Unary - UNA");
            // TODO: I'm not sure if this is enough to make that decision
            //       or if it is needed...
            if (top.type == EXPR_SYMBOL) {
                goto RED;
            }
            goto SHF;
        case EQQ: // '=' symbol
            // push input onto the stack and read the next symbol
            debug("Symbol - EQQ");
            stack_token_t_push(token_stack, input);
            // special treatment for the FUNEXP
            if (top.type == IDENT && input.type == LPAREN) {
                stack_int_push(function_stack, 1);
                stack_int_push(stack_func_ret_types, INVALID);
                debug("DEEPER; NOW AT DEPTH: %d", stack_func_ret_types->top);
            }

            return true;
        SHF:
        case SHF:
            // '<' symbol
            // change 'a' on stack to 'a<'
            debug("Shift - SHF");
            stack_shift(token_stack, top);
            // and push input onto the stack and read the next symbol
            stack_token_t_push(token_stack, input);
            return true;
        RED:
        case RED:
            // '>' symbol
            // if '<y' is on top of the stack and rule exists
            // convert '<y' to A and do semantic actions
            // else, throw syntax error
        debug("Reduce - RED");
            if (!stack_reduce(token_stack, function_stack)) {
                fprintf(stderr, "There was an error while interpreting an expression\n");
                prec_clean(token_stack, function_stack);
                parser_end(ERROR_SYNTAX);
            }
            break;
        default:
            // empty cell
            *stop = true;
            break;
    }
    return false;
}

void parser_expr() {
    debug_entry();

    // start the precedence analysis
    parser_precedence_analysis();
}

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

/* ------------------------------------------------------------------------ */
/* SEMANTIC ACTIONS                                                         */
/* ------------------------------------------------------------------------ */

void parser_track_ident(char *id) {
    debug_entry();
    debug("Tracking a new symbol %s", id);

    symtable_symbol_t *symbol = symtable_add_symbol(st, id);

    symbol->type = IDENT;

    TAC_insert(tac_list, OP_DEFINE, NULL, NULL, TAC_get_identifier(id));
}

void clean_func_calls() {
    debug_entry();
    while (!stack_func_call_t_isempty(stack_func_calls)) {
        func_call_t fc = stack_func_call_t_pop(stack_func_calls);
        func_call_free(&fc);
    }
}

/* ------------------------------------------------------------------------ */
/* THREE ADDRESS CODE GENERATION ACTIONS                                    */
/* ------------------------------------------------------------------------ */

char *create_temporary_variable(token_type type) {
    debug_entry();
    temp_var_cnt++;

    debug("Creating temporary variable with type %s", token_get_type_string(type));

    char *temporary_variable = "$t";
    char *index_string = convert_int_to_string(temp_var_cnt);
    temporary_variable = append(temporary_variable, index_string);
    free (index_string);
    if (temporary_variable == NULL) {
        parser_end(ERROR_INTERNAL);
    }

    // Track the new variable
    parser_track_ident(temporary_variable);
    stack_charptr_push(stack_temp_vars, temporary_variable);

    // Give it a type
    symtable_symbol_t *symbol = symtable_find_symbol(st, temporary_variable);
    if (symbol == NULL) {
        parser_end(ERROR_INTERNAL);
    }
    symbol->token.type = type;

    debug("Current number of temporary variables is %d", stack_temp_vars->top + 1);

    return temporary_variable;
}


void clean_temporary_variables() {
    debug_entry();
    while (!stack_charptr_isempty(stack_temp_vars)) {
        const char *ident = stack_charptr_pop(stack_temp_vars);
        free((char *) ident);
    }
}

char *create_label_name() {
    debug_entry();
    label_cnt++;
    char *label_name = "label$";
    char *index_string = convert_int_to_string(label_cnt);
    label_name = append(label_name, index_string);
    free(index_string);

    return label_name;
}
