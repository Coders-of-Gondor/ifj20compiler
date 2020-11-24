package main

func foo() {
  return 10
}

func main() {
  id := 5 * foo() - (foo() + 1 * (3 - (6 + (-9))))
}
