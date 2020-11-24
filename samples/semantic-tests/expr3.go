package main

func foo(a int, b int) (int) {
  id := a + b
  return id
}

func bar() {
  return
}

func main() {
  id := bar()
}
