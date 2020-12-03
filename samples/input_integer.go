package main

func main() {
	print("Zadejte int hodnotu: ")
	tmp := 0
	err := 0
	tmp, err = inputi()
	
	print(tmp, " ", err)
}
