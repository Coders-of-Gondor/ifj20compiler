#!/usr/bin/env bats

load helpers

readonly semantics_samples="$samples"/semantics-tests

@test "Semantic analysis - assign to undefined variable" {
  run_compiler "$ERROR_SEM_VAR" "$semantics_samples"/var-undefined-assign.go
}

@test "Semantic analysis - use of an undefined variable in expression" {
  run_compiler "$ERROR_SEM_VAR" "$semantics_samples"/var-undefined-in-expr.go
}

@test "Semantic analysis - redefinition of a variable" {
  run_compiler "$ERROR_SEM_VAR" "$semantics_samples"/var-redefinition.go
}

@test "Semantic analysis - use of an undefined function" {
  run_compiler "$ERROR_SEM_VAR" "$semantics_samples"/func-undefined.go
}

@test "Semantic analysis - redefinition of a function" {
  run_compiler "$ERROR_SEM_VAR" "$semantics_samples"/func-redefinition.go
}

@test "Semantic analysis - int to float64" {
  run_compiler "$ERROR_SEM_COMPATIBILITY" "$semantics_samples"/int-to-float.go
}

@test "Semantic analysis - float64 to int" {
  run_compiler "$ERROR_SEM_COMPATIBILITY" "$semantics_samples"/float-to-int.go
}

@test "Semantic analysis - less func parameters than needed" {
  run_compiler "$ERROR_SEM_PROGRAM" "$semantics_samples"/func-less-parameters.go
}

@test "Semantic analysis - more func parameters than needed" {
  run_compiler "$ERROR_SEM_PROGRAM" "$semantics_samples"/func-more-parameters.go
}

@test "Semantic analysis - wrong func parameter" {
  run_compiler "$ERROR_SEM_PROGRAM" "$semantics_samples"/func-wrong-parameter.go
}

@test "Semantic analysis - less return args than needed" {
  run_compiler "$ERROR_SEM_PROGRAM" "$semantics_samples"/func-less-ret-args.go
}

@test "Semantic analysis - more return args than needed" {
  run_compiler "$ERROR_SEM_PROGRAM" "$semantics_samples"/func-more-ret-args.go
}

@test "Semantic analysis - wrong return type" {
  run_compiler "$ERROR_SEM_PROGRAM" "$semantics_samples"/func-wrong-ret-type.go
}

#@test "Semantic analysis - wrong return args type on assign" {
#  run_compiler "$ERROR_SEM_PROGRAM" "$semantics_samples"/func-wrong-ret-args-on-assign.go
#}

@test "Semantic analysis - division by zero" {
  run_compiler "$ERROR_ZERO" "$semantics_samples"/division-by-zero.go
}

