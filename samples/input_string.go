package main

func main() {
	print("Zadejte string hodnotu: ")
	str := ""
	err := 0
	str, err = inputs()
	
	print(str, " ", err)
}
