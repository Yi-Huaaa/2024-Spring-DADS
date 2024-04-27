#ifndef ST_ALGO_H
#define ST_ALGO_H

#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>

// #define ST_CHECK_INPUTS

// fsim namespace
namespace ST {

class Simulator {

public:
  void read(std::ifstream &input_file);
  void run();
  void output_results(std::ofstream &output_file);
  
private:
  size_t _bd_x_0, _bd_x_1, _bd_y_0, _bd_y_1;
  size_t _num_pins;
  std::vector<std::vector<size_t>> _pins;

  std::vector<bool> _direction; // (V, H) = (0, 1)
  std::vector<std::vector<size_t>> _wires; 
  size_t _wire_length;
  size_t _used_num_wires;

  void _update_wires(size_t d0, size_t d1, size_t d2, size_t d3);
  void _assign_wires(size_t num_add_wires, size_t pre_pin_id, size_t now_pin_id);


  
};

} // namespace ST

#endif