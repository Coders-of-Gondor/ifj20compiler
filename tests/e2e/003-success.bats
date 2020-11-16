#!usr/bin/env bats

load helpers

@test "Success analysis - eols.go" {
  run_compiler $samples/eols.go
}

@test "Success analysis - example1.go" {
  run_compiler $samples/example1.go
}

@test "Success analysis - example2.go" {
  run_compiler $samples/example2.go
}

@test "Success analysis - exapmles3.go" {
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

