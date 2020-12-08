package main

func main() {
	a := 0
	b := 0
	for ; a < 5; a = a + 1 {		// a = 5
		b += a		
	}
	print(b, " ")	// 10

	for a = 4; a < 5; a = a + 1 {
		b += a
	} 
	print(b, " ")	// 14
	b = (a*5)/5	
	print(b, "\n")	// 5
	c := 32768
	a = 0
	b = c + a
	print(b)	// 32768
}
