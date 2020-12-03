#!/usr/bin/env bash

# --- CONSTANTS ---
readonly IFJ20COMPILER=${IFJ20COMPILER:-./build/bin/ifj20compiler}
readonly IFJ20INTERPRETER=${IFJ20INTERPRETER:-./ic20int}
readonly BUILD_OUT=${BUILD_OUT:-./build/out}
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

    # helper function to get the compiler rid of stderr
    compiler_without_stderr() {
      timeout --foreground --kill=10 30 $IFJ20COMPILER "$@" 2>/dev/null
      # $IFJ20COMPILER "$@" 2>/dev/null
    }
    run compiler_without_stderr "$@" 3>/dev/null

    if [ "$status" -ne "$expected_rc" ]; then
      echo "exit code is $status; expected $expected_rc" >&2
      false
    fi
}

function run_interpreter() {
  expected_rc=0
    echo ""
    echo "\$ $IFJ20COMPILER $*"

    mkdir -p "$BUILD_OUT"

    # $1 = input file (stdout of the program)
    # $2 = go file (source file)
    # $3 = output file (expected output of the interpreter)
    OUT_GEN="$BUILD_OUT/$(basename $2).gen"
    OUT_FILE="$BUILD_OUT/$(basename $2).out"
    OUT_DIFF="$BUILD_OUT/$(basename $2).diff"

    # helper function to get the inputs and save the outputs
    #compile_and_save_output() {
      #echo "$1"
      #timeout --foreground --kill=10 30 "$IFJ20COMPILER" "$2" 2>/dev/null >"$OUT_GEN"
      #"$IFJ20INTERPRETER" "$OUT_GEN" <"$1" >"$OUT_FILE"
    #}
    timeout --foreground --kill=10 30 "$IFJ20COMPILER" "$2" 2>/dev/null >"$OUT_GEN"
    timeout --foreground --kill=10 30 "$IFJ20INTERPRETER" "$OUT_GEN" <"$1" >"$OUT_FILE"

    echo "Different interpreted outputs. All files located at $BUILD_OUT/"
    echo "Diff: $(basename $OUT_DIFF), Generated code: $(basename $OUT_GEN), interpreted output: $(basename $OUT_FILE)"
    echo "Showing the head:"
    # If sed causes problems, remove it.
    diff <(sed -e '$a\' "$3") <(sed -e '$a\' "$OUT_FILE") | tee "$OUT_DIFF" | head -n 5
    #diff "$3" "$OUT_FILE" | tee "$OUT_DIFF" | head -n 5
    test "${PIPESTATUS[0]}" -eq 0  # trigger the failed status

    if [ "$status" -ne "$expected_rc" ]; then
      echo "exit code is $status; expected $expected_rc" >&2
      false
    fi
}
