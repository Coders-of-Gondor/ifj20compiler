package main

func add(one int, two int) (int) {
	tmp := one + two
	return tmp
}

func main() {
	a := 0
	b := 0
	c := 5
	a = c*c	// 25
	b = a*c	// 125
	d := 0
	d = add(a, b)
	print("Vysledek je: ", d)
}
