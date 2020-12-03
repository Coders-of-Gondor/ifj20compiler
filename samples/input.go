// IFJ20: inputX functions

package main

func main() {
  print("Zadej retezec: ")
  s := ""
  err := 100
  s, err = inputs()
  print("STRING=",s, ". CHYBA=", err, "\n")

  print("Zadej cislo: ")
  i := 420
  i, err = inputi()
  print("INT=",i, ". CHYBA=", err, "\n")

  print("Zadej desetinne cislo: ")
  f := 90.0
  f, err := inputf()
  print("FLOAT64=", f, ". CHYBA=", err, "\n")

}
