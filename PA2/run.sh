#!/bin/bash

set -e

# clear && g++ main.cpp fixed_outline_floorplanning.cpp  -Wall -O3 -std=c++17 -o fp && ./fp input_pa2/1 input_pa2/1.nets output1.txt

g++ main.cpp fixed_outline_floorplanning.cpp -Wall -O3 -std=c++17 -o fp

alpha="0.5"
input_files_1=("1"  "2"  "3"  "ami33"  "ami49"  "apte"  "hp"  "xerox")

if [ $# -eq 0 ]; then
    for input_file in "${input_files_1[@]}"; do
        echo "================================="
        echo "running test data" "input_pa2/${input_file}.block" "input_pa2/${input_file}.nets"
        ./fp $alpha "input_pa2/${input_file}.block" "input_pa2/${input_file}.nets" "output_${input_file}"
        python3 checker/checker.py $input_file output_$input_file
    done
else
    input_id="$1" # Input file number provided by the user
    input_file="${input_id}"
    if [[ ! " ${input_files_1[@]} " =~ " ${input_file} " ]]; then
        echo "Invalid input file number provided."
        exit 1
    fi
    ./fp $alpha "input_pa2/${input_file}.block" "input_pa2/${input_file}.nets" "output_${input_file}"
    python3 checker/checker.py $input_file output_$input_file
    echo "================================="
fi

rm fp
