// IFJ20: Redefinition of a variable

package main

func testFunc() (int) {
  return 42, 3.14
}

func main() {
  a := testFunc()
}
