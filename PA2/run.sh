#!/bin/bash

set -e

g++ main.cpp fixed_outline_floorplanning.cpp -Wall -O3 -std=c++17 -o fofp
g++ main.cpp fixed_outline_floorplanning.cpp -Wall -O3 -std=c++17 -D debug_on -o fofp_debug

input_array=("1" "2" "3" "ami33" "ami49" "apte" "hp" "xerox" )

if [ $# -eq 0 ]; then
    for file in "${input_array[@]}"; do
        echo "================================="
        echo "running test data: ${file}"            
        ./fofp input_pa2/${file}.block input_pa2/${file}.nets output_${file}.rpt
        ./checker/checker.py ${file} output_${file}.rpt
    echo ""
    done;
else
    input_id="$1" # Input file number provided by the user
    ./fofp input_pa2/${input_id}.block input_pa2/${input_id}.nets output_${input_id}.rpt

    echo "================================="

    ./checker/checker.py ${input_id} output_${input_id}.rpt
fi

rm fofp fofp_debug
