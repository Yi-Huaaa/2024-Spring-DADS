#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <vector>
#include "./fiduccia_mattheyses_algorithm.hpp"


int main(int argc, char *argv[]) {
  std::string input_file_path(argv[1]);
  std::string output_file_path(argv[2]);
  std::ifstream input_file(input_file_path);
  std::ofstream output_file(output_file_path);
  
  // check open file
  using std::string_literals::operator""s;
  if (!input_file) {
    throw std::runtime_error("cannot open circut file "s + input_file_path);
  }
  if (!output_file) {
    throw std::runtime_error("cannot open circut file "s + output_file_path);
  }
  
  FM::Simulator simulator;
  std::chrono::duration<double> duration;

auto start = std::chrono::steady_clock::now();
  simulator.read(input_file);
  simulator.run();
  simulator.output_results(output_file); 
auto end = std::chrono::steady_clock::now();

  duration = end - start;
  // std::cout << "duration: " << (duration.count())*1000 << " ms\n";

  return 0;
}

