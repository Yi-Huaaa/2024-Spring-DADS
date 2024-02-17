#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <vector>
#include "./fiduccia_mattheyses_algorithm.hpp"


int main(int argc, char *argv[]) {
  std::string file_path(argv[1]);
  std::ifstream input_file (file_path);
  
  // check open file
  using std::string_literals::operator""s;
  if (!input_file) {
    throw std::runtime_error("cannot open circut file "s + file_path);
  }
  
  FM::Simulator simulator;
  std::chrono::duration<double> duration;

auto start = std::chrono::steady_clock::now();
  simulator.read(input_file);
  // simulator.run(); 
  // simulator.get_results(); 
auto end = std::chrono::steady_clock::now();

  duration = end - start;
  std::cout << "duration: " << (duration.count())*1000 << " ms\n";

  return 0;
}

