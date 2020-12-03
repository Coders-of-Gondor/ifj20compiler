package main

func main() {
	print("Zadejte float hodnotu: ")
	flt := 0.0
	err := 0
	flt, err = inputf()
	
	print(flt, " ", err)
}
