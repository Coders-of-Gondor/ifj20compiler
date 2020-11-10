// IFJ20: inputX functions

package main

func main() {
  print("Zadej retezec: ")
  s, err := inputs()
  print("STRING=",s, ". CHYBA=", err, "\n")

  print("Zadej cislo: ")
  i, err := inputi()
  print("INT=",i, ". CHYBA=", err, "\n")

  print("Zadej desetinne cislo: ")
  f, err := inputf()
  print("FLOAT64=", f, ". CHYBA=", err, "\n")

}
