// IFJ20: Redefinition of a variable

package main

func testFunc(arg1 int, arg2 float64) {
  a := arg1
  b := arg2
}

func main() {
  testFunc(1, 1.5, "arg")
}
