#!/bin/bash

set -e

clear && g++ main.cpp steiner_tree_onstruction.cpp -Wall -O3 -std=c++17 -o st

input_files_1=("case1" "case2" "case3" "case5" "case6" "case8" "case100000" "case200000" "case4" "case500000" "case7")

if [ $# -eq 0 ]; then
    for input_file in "${input_files_1[@]}"; do
        echo "================================="
        echo "running test data" "input_pa3/${input_file}"
        ./st "input_pa3/${input_file}" "output_${input_file}"
    done
else
    input_id="$1" # Input file number provided by the user
    input_file="${input_id}"
    if [[ ! " ${input_files_1[@]} " =~ " ${input_file} " ]]; then
        echo "Invalid input file number provided."
        exit 1
    fi
    ./st "input_pa3/${input_file}" "output_${input_file}"
    echo "================================="
fi

rm st
