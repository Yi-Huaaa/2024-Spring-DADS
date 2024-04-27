#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <vector>
#include "./steiner_tree_onstruction.hpp"

#define ST_SHOW_TIME

int main(int argc, char *argv[]) {
  std::string input_file_path(argv[1]); // input file
  std::string output_file_path(argv[2]); // output file
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
  
  ST::Simulator simulator;
  std::chrono::duration<double> duration;

auto start = std::chrono::steady_clock::now();
  simulator.read(input_file);
  simulator.run();
auto end = std::chrono::steady_clock::now();
  duration = end - start;
  simulator.output_results(output_file);
  
  
#ifdef ST_SHOW_TIME
  std::cout << "duration = " << duration.count() << "s" << std::endl;
  // double duration_seconds = duration.count();
  // printf("duration = %.2lf seconds\n", duration_seconds);
#endif

  return 0;
}

