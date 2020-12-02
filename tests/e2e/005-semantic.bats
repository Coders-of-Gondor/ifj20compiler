#!/usr/bin/env bats

load helpers

readonly semantic_samples="$samples/semantic-tests"

# Tests for expression concerning invalid number of parameters / return values
@test "Semantic analysis - expr1" {
  run_compiler $ERROR_SEM_PROGRAM $semantic_samples/expr1.go
}

@test "Semantic analysis - expr2" {
  run_compiler $ERROR_SEM_PROGRAM $semantic_samples/expr2.go
}

@test "Semantic analysis - expr3" {
  run_compiler $ERROR_SEM_PROGRAM $semantic_samples/expr3.go
}

@test "Semantic analysis - expr4" {
  run_compiler $ERROR_SEM_PROGRAM $semantic_samples/expr4.go
}

@test "Semantic analysis - expr5" {
  run_compiler $ERROR_SEM_VAR $semantic_samples/expr5.go
}
