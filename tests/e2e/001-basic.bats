#!/usr/bin/env bats

load helpers

@test "Show usage screen when no command is given" {
  run_compiler 1
}

@test "Run compiler with a valid ifj20 source file" {
  run_compiler $samples/example1.go 
  [ "$output" = "" ]
}

@test "Run compiler with a non-existent file" {
  run_compiler 99 $samples/invalidfile.go
}
