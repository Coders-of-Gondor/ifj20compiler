#!/usr/bin/env bash

# generate_stacks.bash
# Marek Filip <xfilip46>
# Implementace překladače imperativního jazyka IFJ20.
# 26/11/2020

# Usage: ./generate_stacks.bash from the ROOT folder (ifj20compiler)
# This script will generate a stack for every entry in the scripts/stacks.txt
# Each word in stacks.txt is an argument to the generate_stack_type.bash script.

root=ifj20compiler
currentdir=$(basename $(pwd))

if [[ "$currentdir" -ne "$root" ]]; then
	echo "Script probably not run from the root folder."
	echo "If this is mistake, please change the 'root' variable to your's root folder."
fi

input="scripts/stacks.txt"

# read the file and put each line into an array
while read -ra line; do
	# run 
	bash ./scripts/generate-stack-type.bash "${line[0]}" "${line[1]}" "${line[@]:2}"
done < "$input"

echo "Don't forget to add the source files into the Makefile!!!"
