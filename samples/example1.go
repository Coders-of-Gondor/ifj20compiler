// Program 1: Vypocet faktorialu (iterativne)
package main

func main() {
  print("Zadejte cislo pro vypocet faktorialu: ")
  a := 0
	a, _ = inputi()
  if a < 0 {
    print("Faktorial nejde spocitat!\n")
  } else {
    vysl := 1
    for ; a > 0; a = a - 1 {
      vysl = vysl * a
    }
    print("Vysledek je ", vysl, "\n")
  }
}
