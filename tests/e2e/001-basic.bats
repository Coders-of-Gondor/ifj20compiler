#!/usr/bin/env bats

load helpers

@test "Run compiler with a non-existent file" {
  run_compiler $ERROR_INTERNAL $samples/invalidfile.go
}

@test "Run compiler with a valid ifj20 source file" {
  run_compiler $SUCCESS $samples/example1.go
}

@test "Run compiler with a valid ifj20 source file passed through stdin" {
  run_compiler $SUCCESS <$samples/example1.go
}

