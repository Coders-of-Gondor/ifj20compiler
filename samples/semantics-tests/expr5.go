// '_' error

package main

func foo(a int, b int) (int) {
  id := a + b
  return id
}

func main() {
  id := foo(_, b, 5)
}
