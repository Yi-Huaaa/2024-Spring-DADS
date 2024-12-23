#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <vector>
#include "./fixed_outline_floorplanning.hpp"

int main(int argc, char *argv[]) {
  double alpha = stod(std::string(argv[1]));
  std::string input_file_path_1(argv[2]); // input.block
  std::string input_file_path_2(argv[3]); // input.nets
  std::string output_file_path(argv[4]); // output file
  std::ifstream input_file_1(input_file_path_1);
  std::ifstream input_file_2(input_file_path_2);
  std::ofstream output_file(output_file_path);
  
  // check open file
  using std::string_literals::operator""s;
  if (!input_file_1) {
    throw std::runtime_error("cannot open circut file "s + input_file_path_1);
  }
  if (!input_file_2) {
    throw std::runtime_error("cannot open circut file "s + input_file_path_2);
  }
  if (!output_file) {
    throw std::runtime_error("cannot open circut file "s + output_file_path);
  }
  
  FP::Simulator simulator;
  std::chrono::duration<double> duration;

auto start = std::chrono::steady_clock::now();
  simulator.read(input_file_1, input_file_2);
  simulator.run(alpha);
auto end = std::chrono::steady_clock::now();
  duration = end - start;
  simulator.output_results(output_file, duration.count());
  // std::cout << "duration: " << duration.count() * 1000 << " ms\n";
  return 0;
}

