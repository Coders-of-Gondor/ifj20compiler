package main

func foo(a int, b int) (int) {
  val := a + b
  return val
}

func main() {
  a := 3
  b := 5

  id := (5) * foo(a - 5, (b * 3) - 2) - (foo(a + 10, b / 12 + foo(2, 3) * 4))
}
