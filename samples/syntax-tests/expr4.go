package main

func hello() {
  return 5
}

func stop() {
  return 6
}

func main() {
  id:=hello() stop()
}
