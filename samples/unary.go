// IFJ 20 code with the unary extension used
// Made as a part of scanner unit tests

package main

func main() {
	a := 1	// a is an int
	a += 1	// 2
	b := 1.5  // b is a float
	a -= 1  // 1
	a *= 5  // 5
	a /= 5  // 1
	a = +a  // (0 + a, 1)
	a = -a  // (0 - a, -1)
	str := "text"
}
