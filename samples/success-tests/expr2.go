package main

func foo() (int) {
  return 20
}

func main() {
  id := 5 * foo() - foo() + 1
}
