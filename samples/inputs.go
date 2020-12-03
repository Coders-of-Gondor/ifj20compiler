// IFJ20: inputs n-times

package main

func main() {
  print("Zadej pocet radku k nacteni: ")
  n := 0
  n, _ = inputi()
  print("Nacitame ", n, " radku...\n")

  for i := 1; i <= n; i = i + 1 {
    print("Zadej retezec c. ", i, ": ")
    s := ""
    err := 0
    s, err = inputs()
    print("STRING", i, "=", s, ".", " CHYBA=", err, "\n")  
  }
  
}
