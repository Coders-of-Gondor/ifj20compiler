package main

func foo(a int, b int) (int) {
  return 10
}

func main() {
  a := 3
  b := 5

  id := 5 * foo(a, b) - (foo(a, b) + 1 * (3 - (6 + (-9))))
}
