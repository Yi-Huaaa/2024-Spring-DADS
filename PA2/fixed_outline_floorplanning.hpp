#ifndef FOFP_ALGO_H
#define FOFP_ALGO_H

#include <iostream>
#include <list>
#include <vector>

// fsim namespace
namespace FOFP {

class Simulator {

public:
  void read(std::ifstream &input_file);
  void run();
  void output_results(std::ofstream &output_file);

private:

};

  

} // namespace FOFP




#endif