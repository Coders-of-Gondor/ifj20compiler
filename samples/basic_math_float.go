package main

func main() {
	a := 10.5
	a = a - 5.3	// 5.2
	b := a + 1.1 // 6.3	
	a = a / 5.2	// 1
	b = b * 2.5	// 15.75
	print(a, " ", b)	// perhaps b will be problematic a bit  - it might be 0x1.f8p+3 or 0x1.f800000000001p+3
}
