// IFJ20: Redefinition of a variable

package main

func testFunc() (int) {
  return "ret arg"
}

func main() {
  a := testFunc()
}
