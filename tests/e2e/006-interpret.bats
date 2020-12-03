load helpers

@test "Interpret analysis - hello.go" {
  run_interpreter $samples/hello.in $samples/hello.go $samples/hello.out
}

@test "Interpret analysis - hello2.go" {
  run_interpreter $samples/hello2.in $samples/hello2.go $samples/hello2.out
}

@test "Interpret analysis - hello3.go" {
  run_interpreter $samples/hello3.in $samples/hello3.go $samples/hello3.out
}

@test "Interpret analysis - hello4.go" {
  run_interpreter $samples/hello4.in $samples/hello4.go $samples/hello4.out
}

@test "Interpret analysis - hello5.go" {
  run_interpreter $samples/hello5.in $samples/hello5.go $samples/hello5.out
}

@test "Interpret analysis - print_float.go" {
  run_interpreter $samples/print_float.in $samples/print_float.go $samples/print_float.out
}

@test "Interpret analysis - print_string.go" {
  run_interpreter $samples/print_string.in $samples/print_string.go $samples/print_string.out
}

@test "Interpret analysis - print_integer.go" {
  run_interpreter $samples/print_integer.in $samples/print_integer.go $samples/print_integer.out
}

@test "Interpret analysis - input_integer.go" {
  run_interpreter $samples/input_integer.in $samples/input_integer.go $samples/input_integer.out
}

@test "Interpret analysis - input_float.go" {
  run_interpreter $samples/input_float.in $samples/input_float.go $samples/input_float.out
}

@test "Interpret analysis - input_string.go" {
  run_interpreter $samples/input_string.in $samples/input_string.go $samples/input_string.out
}

@test "Interpret analysis - basic_math_int.go" {
  run_interpreter $samples/basic_math_int.in $samples/basic_math_int.go $samples/basic_math_int.out
}

@test "Interpret analysis - basic_math_float.go" {
  run_interpreter $samples/basic_math_float.in $samples/basic_math_float.go $samples/basic_math_float.out
}

@test "Interpret analysis - advanced_math_int.go" {
  run_interpreter $samples/advanced_math_int.in $samples/advanced_math_int.go $samples/advanced_math_int.out
}

@test "Interpret analysis - math_in_function.go" {
  run_interpreter $samples/math_in_function.in $samples/math_in_function.go $samples/math_in_function.out
}

@test "Interpret analysis - example1.go" {
  run_interpreter $samples/example1.in $samples/example1.go $samples/example1.out
}

@test "Interpret analysis - example2.go" {
  run_interpreter $samples/example2.in $samples/example2.go $samples/example2.out
}

@test "Interpret analysis - example3.go" {
  run_interpreter $samples/example3.in $samples/example3.go $samples/example3.out
}

@test "Interpret analysis - input.go" {
  run_interpreter $samples/input.in $samples/input.go $samples/input.out
}

@test "Interpret analysis - inputs.go" {
  run_interpreter $samples/inputs.in $samples/inputs.go $samples/inputs.out
}

@test "Interpret analysis - scope.go" {
  run_interpreter $samples/scope.in $samples/scope.go $samples/scope.out
}

@test "Interpret analysis - two-pass.go" {
  run_interpreter $samples/two-pass.in $samples/two-pass.go $samples/two-pass.out
}

@test "Interpret analysis - varfun.go" {
  run_interpreter $samples/varfun.in $samples/varfun.go $samples/varfun.out
}

@test "Interpret analysis - eols.go" {
  run_interpreter $samples/eols.in $samples/eols.go $samples/eols.out
}
