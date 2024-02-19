#!/bin/bash

set -e

g++ main.cpp fiduccia_mattheyses_algorithm.cpp -Wall -O3 -std=c++17 -o fm
g++ main.cpp fiduccia_mattheyses_algorithm.cpp -Wall -O3 -std=c++17 -D debug_on -o fm_debug

if [ $# -eq 0 ]; then
    for input_id in $(seq 0 8); do
    echo "================================="
    echo "running test data $input_id"
    ./fm input_pa1/input_${input_id}.dat output_${input_id}.dat
    ./checker/checker_linux input_pa1/input_${input_id}.dat output_${input_id}.dat
    done;
else
    input_id="$1" # Input file number provided by the user
    ./fm input_pa1/input_${input_id}.dat output_${input_id}.dat

    echo "================================="
    ./checker/checker_linux input_pa1/input_${input_id}.dat output_${input_id}.dat
fi

# rm fm

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