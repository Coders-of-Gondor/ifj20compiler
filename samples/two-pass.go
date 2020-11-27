// IFJ20: Ukazka volani funkce f pred jeji definici

package main

func g() {
	j := 0
	j = f(3)
	j = f(j)
}

func f(i int) (int) {
	return i
}

func main() {
  g()
}

