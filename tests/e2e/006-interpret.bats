load helpers

@test "Interpret analysis - hello.go" {
  run_interpreter $samples/hello.in $samples/hello.go $samples/hello.out
}
