// IFJ20: Redefinition of a variable

package main

func testFunc() (int) {
  return
}

func main() {
  a := testFunc()
}
