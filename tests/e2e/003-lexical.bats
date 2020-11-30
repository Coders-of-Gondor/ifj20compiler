#!/usr/bin/env bats

load helpers

readonly lexical_samples="$samples/lexical-tests"

@test "Lexical analysis - colon" {
  run_compiler $ERROR_LEXICAL $lexical_samples/colon.go
}

@test "Lexical analysis - exclamation mark" {
  run_compiler $ERROR_LEXICAL $lexical_samples/excl_mark.go
}

@test "Lexical analysis - integer1" {
	run_compiler $ERROR_LEXICAL $lexical_samples/integer1.go
}

@test "Lexical analysis - integer2" {
	run_compiler $ERROR_LEXICAL $lexical_samples/integer2.go
}

@test "Lexical analysis - integer3" {
	run_compiler $ERROR_LEXICAL $lexical_samples/integer3.go
}

@test "Lexical analysis - integer4" {
	run_compiler $ERROR_LEXICAL $lexical_samples/integer4.go
}

@test "Lexical analysis - integer5" {
	run_compiler $ERROR_LEXICAL $lexical_samples/integer5.go
}

@test "Lexical analysis - integer6" {
	run_compiler $ERROR_LEXICAL $lexical_samples/integer6.go
}

@test "Lexical analysis - integer7" {
	run_compiler $ERROR_LEXICAL $lexical_samples/integer7.go
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

@test "Lexical analysis - float9" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float9.go
}

@test "Lexical analysis - float10" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float10.go
}

@test "Lexical analysis - float11" {
  run_compiler $ERROR_LEXICAL $lexical_samples/float11.go
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

@test "Lexical analysis - unending_comment" {
  run_compiler $ERROR_LEXICAL $lexical_samples/unending.go
}

@test "Lexical analysis - block_comment_in_block" {
  run_compiler $ERROR_LEXICAL $lexical_samples/block_in_block.go
}
