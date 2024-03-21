#!/bin/bash

set -e

g++ main.cpp fiduccia_mattheyses_algorithm.cpp -Wall -O3 -std=c++17 -o fm
g++ main.cpp fiduccia_mattheyses_algorithm.cpp -Wall -O3 -std=c++17 -D debug_on -o fm_debug

input_files=("aes_core.dat" "tv80.dat" "vga_lcd.dat" "0.dat" "1.dat" "2.dat" "3.dat" "4.dat" "5.dat" "6.dat" "7.dat" "8.dat")

input_files=("aes_core.dat" "tv80.dat" "vga_lcd.dat" "0.dat" "1.dat" "2.dat" "3.dat" "4.dat" "5.dat" "6.dat")
# input_files=("aes_core.dat" "tv80.dat" "vga_lcd.dat")

if [ $# -eq 0 ]; then
    for input_file in "${input_files[@]}"; do
        echo "================================="
        echo "running test data input_${input_file}"
        /usr/bin/time -f "%e\n%M"  ./fm "input_pa1/input_${input_file}" "output_${input_file}"
        ./checker/checker_linux "input_pa1/input_${input_file}" "output_${input_file}"
    done
else
    input_id="$1" # Input file number provided by the user
    input_file="input_${input_id}.dat"
    if [[ ! " ${input_files[@]} " =~ " ${input_file} " ]]; then
        echo "Invalid input file number provided."
        exit 1
    fi
    ./fm "input_pa1/input_${input_id}" "output_${input_file}"
    echo "================================="
    ./checker/checker_linux "input_pa1/input_${input_id}" "output_${input_file}"
fi

rm fm fm_debug


# ychung79@twhuang-desktop-03:~/DADS/2024-Spring-DADS/PA1$ ll input_pa1/
# total 34316
# drwxr-xr-x 2 ychung79 pvt-ychung79     4096 Feb 17 23:19 ./
# drwxr-xr-x 4 ychung79 pvt-ychung79     4096 Feb 17 23:54 ../
# -rw-r--r-- 1 ychung79 pvt-ychung79  8031907 Feb 16 15:17 input_0.dat
# -rw-r--r-- 1 ychung79 pvt-ychung79   129258 Feb 16 15:17 input_1.dat
# -rw-r--r-- 1 ychung79 pvt-ychung79   264542 Feb 16 15:17 input_2.dat
# -rw-r--r-- 1 ychung79 pvt-ychung79  2967749 Feb 16 15:17 input_3.dat
# -rw-r--r-- 1 ychung79 pvt-ychung79  5868688 Feb 16 15:17 input_4.dat
# -rw-r--r-- 1 ychung79 pvt-ychung79 17848057 Feb 16 15:17 input_5.dat
# -rw-r--r-- 1 ychung79 pvt-ychung79       92 Feb 16 15:17 input_6.dat
# -rw-r--r-- 1 ychung79 pvt-ychung79       89 Feb 17 17:33 input_7.dat
# -rw-r--r-- 1 ychung79 pvt-ychung79      127 Feb 17 23:20 input_8.dat