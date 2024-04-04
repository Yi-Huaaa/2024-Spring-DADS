# PA2

## How to Compile

```
g++ main.cpp fixed_outline_floorplanning.cpp -Wall -O3 -std=c++17 -o fp
```

## How to Execute

```
./fp [alpha] [input file_0_block path] [input file_1_nets path] [output file path]
python3 checker/checker.py $input_file output_$input_file
```

### Example

```
# alpha = 0.5
# run && check
./fp 0.5 input_pa2/1.block input_pa2/1.nets output_1
python3 checker/checker.py 1 output_1
```