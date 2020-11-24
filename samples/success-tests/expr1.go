package main

func foo() (int) {
  return 10
}

func main() {
  id := 5 + 1 - foo()
}
