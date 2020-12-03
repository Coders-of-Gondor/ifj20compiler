package main

func foo(a int, b int) (int) {
  val := a + b
  return val
}

func main() {
  a := 3
  b := 5
  a, b = foo(a, b), foo(a, b, foo(10+(3), 100*243))
}
