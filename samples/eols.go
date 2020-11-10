// IFJ20: Ilustrace, kde se mohou, kde musi a kde se nesmi vyskytovat odradkovani
package main

func add(i /*zde se nesmi odradkovat*/ int,
	j int /*zde se nesmi odradkovat*/) (int) /*zde se nesmi odradkovat*/ {
	return i + j
}

func main() /*zde se nesmi odradkovat*/ {
	i := 0
	i = add( /* zde muzeme odradkovavat*/
		1, /*pred carkou se nesmi, za carkou muze odradkovat*/
		1 /*zde se nesmi odradkovat*/)
	print(i, "\n")
	i = /* zde muzeme odradkovavat*/
		1 /*zde se nesmi odradkovat*/ + ( /* zde muzeme odradkovavat*/
		1) /*zde se nesmi odradkovat*/ *  /* zde muzeme odradkovavat*/
			(2) /*zde se musi odradkovat*/
	if i > /* zde muzeme odradkovavat*/
		0 /*zde se nesmi odradkovat*/ { /*zde se musi odradkovat*/
		print(i)
	} else /*zde se nesmi odradkovat*/ /*zde se nesmi odradkovat*/ { /*zde se musi odradkovat*/
	} /*zde se musi odradkovat*/
}
