#!/usr/bin/env bash

IFJ20COMPILER=${IFJ20COMPILER:-./build/bin/ifj20compiler}
samples="./samples"

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
