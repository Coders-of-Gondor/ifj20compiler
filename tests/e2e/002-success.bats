#!usr/bin/env bats

load helpers

readonly success_sample="$samples/success-tests"

@test "Success analysis - eols.go" {
  run_compiler $samples/eols.go
}

@test "Success analysis - example1.go" {
  run_compiler $samples/example1.go
}

@test "Success analysis - example2.go" {
  run_compiler $samples/example2.go
}

@test "Success analysis - exapmle3.go" {
  run_compiler $samples/example3.go
}

@test "Success analysis - hello.go" {
  run_compiler $samples/hello.go
}

@test "Success analysis - scope.go" {
  run_compiler $samples/scope.go
}

@test "Success analysis - two-pass.go" {
  run_compiler $samples/two-pass.go
}

@test "Success analysis - varfun.go" {
  run_compiler $samples/varfun.go
}

@test "Success analysis - math-operations.go" {
  run_compiler $samples/math-operations.go
}

@test "Success analysis - unary.go" {
  run_compiler $samples/unary.go
}

@test "Success analysis - divide.go" {
  run_compiler $samples/divide.go
}

@test "Sucess analysis - string_escape_hex" {
  run_compiler $samples/string_escape_hex.go
}

@test "success analysis - underscore" {
  run_compiler $samples/underscore.go
}

# test the success of precedence parsing
@test "Sucess analysis - expr1" {
  run_compiler $success_sample/expr1.go
}

@test "Sucess analysis - expr2" {
  run_compiler $success_sample/expr2.go
}

@test "Sucess analysis - expr3" {
  run_compiler $success_sample/expr3.go
}

@test "Sucess analysis - expr4" {
  run_compiler $success_sample/expr4.go
}

@test "Sucess analysis - expr5" {
  run_compiler $success_sample/expr5.go
}

@test "Sucess analysis - expr6" {
  run_compiler $success_sample/expr6.go
}

@test "Sucess analysis - expr7" {
  run_compiler $success_sample/expr7.go
}

@test "Sucess analysis - eol1" {
  run_compiler $success_sample/eol1.go
}
