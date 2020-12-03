package main

func main() {
  a := 5
  // 1 alone should throw type error
  if (1) {
    a = 1
  } else {
    a = 2
  }
}

