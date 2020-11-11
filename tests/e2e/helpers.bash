#!/usr/bin/env bash

# --- CONSTANTS ---
readonly IFJ20COMPILER=${IFJ20COMPILER:-./build/bin/ifj20compiler}
readonly samples="./samples"

# error codes constants
readonly SUCCESS=0                   # No errors
readonly ERROR_LEXICAL=1             # Lexeme error
readonly ERROR_SYNTAX=2              # Error in syntax
readonly ERROR_SEM_VAR=3             # Semantic error - undefined variable, attempt to redefine variable...
readonly ERROR_SEM_VAR_TYPE=4        # Semantic error - invalid data type of newly defined variable
readonly ERROR_SEM_COMPATIBILITY=5   # Semantic error - incompatible data types in arithmetic or string operations, or relational expression
readonly ERROR_SEM_PROGRAM=6         # Semantic error - invalid number/types of parameters and/or return values
readonly ERROR_SEM=7                 # Other semantic errors
readonly ERROR_ZERO=9                # Semantic error - dividing by zero
readonly ERROR_INTERNAL=99           # Internal error - memory errors...
# --- CONSTANTS END ---

function run_compiler() {
  expected_rc=0
  case "$1" in
    [0-9])
      expected_rc=$1
      shift
      ;;
    [1-9][0-9])
      expected_rc=$1
      shift
      ;;
    [1-9][0-9][0-9])
      expected_rc=$1
      shift
      ;;
    esac

    echo ""
    echo "\$ $IFJ20COMPILER $*"
    run timeout --foreground --kill=10 30 $IFJ20COMPILER "$@" 3>/dev/null

    if [ -n "$output" ]; then
      echo "$output"
      echo ""
    fi

    if [ "$status" -ne "$expected_rc" ]; then
      echo "exit code is $status; expected $expected_rc" >&2
      false
    fi
}
