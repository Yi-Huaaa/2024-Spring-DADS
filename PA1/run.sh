#!/bin/bash

set -e

if [ $# -eq 0 ]; then
    input_file="6" # Default input file number
else
    input_file="$1" # Input file number provided by the user
fi

g++ main.cpp fiduccia_mattheyses_algorithm.cpp -Wall -O3 -std=c++17 -o fm

./fm input_pa1/input_${input_file}.dat output_${input_file}.dat

rm fm