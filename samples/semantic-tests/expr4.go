package main

func foo(a int, b int) (int, string) {
  id := a + b
  return id
}

func main() {
  // can I call the function like that? -probably yes
  foo()
}
