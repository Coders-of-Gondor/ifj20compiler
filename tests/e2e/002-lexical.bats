#!/usr/bin/env bats

load helpers

lexical_samples="$samples/lexical-tests"

@test "Lexical analysis - colon" {
  run_compiler $ERROR_LEXICAL $lexical_samples/colon.go
}

@test "Lexical analysis - exclamation mark" {
  run_compiler $ERROR_LEXICAL $lexical_samples/excl_mark.go
}

@test "Lexical analysis - float1" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float1.go
}

@test "Lexical analysis - float2" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float2.go
}

@test "Lexical analysis - float3" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float3.go
}

@test "Lexical analysis - float4" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float4.go
}

@test "Lexical analysis - float5" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float5.go
}

@test "Lexical analysis - float6" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float6.go
}

@test "Lexical analysis - float7" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float7.go
}

@test "Lexical analysis - float8" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float8.go
}

@test "Lexical analysis - string1" {
  run_compiler $ERROR_LEXICAL $lexical_samples/string1.go
}

@test "Lexical analysis - string2" {
  run_compiler $ERROR_LEXICAL $lexical_samples/string2.go
}

@test "Lexical analysis - string3" {
  run_compiler $ERROR_LEXICAL $lexical_samples/string3.go
}

@test "Lexical analysis - string4" {
  run_compiler $ERROR_LEXICAL $lexical_samples/string4.go
}

