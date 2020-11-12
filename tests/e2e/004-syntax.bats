#!/usr/bin/env bats

load helpers

readonly syntax_samples="$samples"/syntax-tests

@test "Syntax analysis - package1" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/package1.go
}

@test "Syntax analysis - package2" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/package2.go
}

@test "Syntax analysis - package3" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/package3.go
}

@test "Syntax analysis - func1" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/func1.go
}

@test "Syntax analysis - func2" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/func2.go
}

@test "Syntax analysis - func3" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/func3.go
}

@test "Syntax analysis - func4" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/func4.go
}

@test "Syntax analysis - expr1" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/expr1.go
}

@test "Syntax analysis - expr2" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/expr2.go
}

@test "Syntax analysis - expr3" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/expr3.go
}

@test "Syntax analysis - expr4" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/expr4.go
}

@test "Syntax analysis - control1" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/control1.go
}

@test "Syntax analysis - control2" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/control2.go
}

@test "Syntax analysis - control3" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/control3.go
}

@test "Syntax analysis - control4" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/control4.go
}

@test "Syntax analysis - general1" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/general1.go
}

@test "Syntax analysis - general2" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/general2.go
}

@test "Syntax analysis - general3" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/general3.go
}

@test "Syntax analysis - general4" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/general4.go
}

@test "Syntax analysis - general5" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/general5.go
}

@test "Syntax analysis - param1" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/param1.go
}

@test "Syntax analysis - param2" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/param2.go
}

@test "Syntax analysis - param3" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/param3.go
}

@test "Syntax analysis - param4" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/param4.go
}

@test "Syntax analysis - eol1" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/eol1.go
}

@test "Syntax analysis - eol2" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/eol2.go
}

@test "Syntax analysis - eol3" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/eol3.go
}

@test "Syntax analysis - eol4" {
  run_compiler "$ERROR_SYNTAX" "$syntax_samples"/eol4.go
}
