// IFJ20: Ukazka prekryti promenne y a upozorneni na nebezpeci prekryti iteracni promenne i

package main

func main() {
	y := 20
	if 1 == 1 {
		y := 10
		print(y)
	} else {
		print(y)
	}
	for i := 1; i <= 10; {
		print(i)
		i = i + 5 // POZOR na i := i + 5, nekonecna smycka
	}
	print(y)
}
