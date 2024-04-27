#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <cstddef>
#include <stack>
#include <queue>
#include <algorithm> // for std::sort

#include "./steiner_tree_onstruction.hpp"

// public functions
void ST::Simulator::read(std::ifstream &input_file) {
  // input file
  std::string line;
  
  std::getline(input_file, line);
  sscanf(line.c_str(), "Boundary = (%zu,%zu), (%zu,%zu)", &_bd_x_0, &_bd_y_0, &_bd_x_1, &_bd_y_1);
  
  std::getline(input_file, line);
  sscanf(line.c_str(), "NumPins = %zu", &_num_pins);

  _pins.resize(_num_pins);
  for (size_t i = 0; i < _num_pins; i++) {
    _pins[i].resize(2);
    std::getline(input_file, line);
    char tmp[1024];
    sscanf(line.c_str(), "PIN %s (%zu,%zu)", tmp, &_pins[i][0], &_pins[i][1]);
    // if (_pins[i][0] < _bd_x_0 || _pins[i][0] > _bd_x_1 || _pins[i][1] < _bd_y_0 || _pins[i][1] > _bd_y_1) {
    //   std::cout << "_pins[i][0] = " << _pins[i][0] << std::endl;
    //   std::cout << "_pins[i][1] = " << _pins[i][1] << std::endl;
    //   std::cout << "OOB!!\n";
    // }
  }
    
#ifdef ST_CHECK_INPUTS
  printf("Boundary: (%ld, %ld), (%ld, %ld)\n", _bd_x_0, _bd_x_1, _bd_y_0, _bd_y_1);
  printf("_num_pins = %ld\n",  _num_pins);

  for (size_t i = 0; i < _num_pins; i++) {
    printf("PIN p%ld, (%ld,%ld)\n", i, _pins[i][0], _pins[i][1]);
  }
#endif
}

void ST::Simulator::_update_wires(size_t d0, size_t d1, size_t d2, size_t d3) {
  _wires[_used_num_wires].resize(4);
  _wires[_used_num_wires][0] = d0;
  _wires[_used_num_wires][1] = d1;
  _wires[_used_num_wires][2] = d2;
  _wires[_used_num_wires][3] = d3;
  _used_num_wires++;
}

void ST::Simulator::_assign_wires(size_t num_add_wires, size_t pre_pin_id, size_t now_pin_id) {
  size_t x0 = _pins[pre_pin_id][0];
  size_t y0 = _pins[pre_pin_id][1];
  size_t x1 = _pins[now_pin_id][0];
  size_t y1 = _pins[now_pin_id][1];

  if (num_add_wires == 1) { // add one wires
    _update_wires(((x0 < x1) ? (x0) : (x1)), ((y0 < y1) ? (y0) : (y1)), 
                  ((x0 > x1) ? (x0) : (x1)), ((y0 > y1) ? (y0) : (y1)));
    if (x0 == x1) {
      _direction[_used_num_wires] = 0; // V
      _wire_length += abs(y1-y0);
    } else { // y0 == y1
      _direction[_used_num_wires] = 1; // H
      _wire_length += abs(x1-x0);
    }
  } else { // add two wires, 4 cases
    if (x0 < x1 && y0 < y1) { // case 0
      _update_wires(x0, y0, x1, y0);
      _wire_length += abs(x1-x0);
      _direction[_used_num_wires] = 1; // H

      _update_wires(x1, y0, x1, y1);
      _wire_length += abs(y1-y0);  
      _direction[_used_num_wires] = 0; // V
    } else if (x0 < x1 && y0 > y1) { // case 1
      _update_wires(x0, y1, x0, y0);
      _wire_length += abs(x1-x0);
      _direction[_used_num_wires] = 0; // V

      _update_wires(x0, y1, x1, y1);
      _wire_length += abs(y1-y0);    
      _direction[_used_num_wires] = 1; // H
    } else if (x1 < x0 && y1 < y0) { // case 2
      _update_wires(x1, y1, x0, y1);
      _wire_length += abs(x1-x0);
      _direction[_used_num_wires] = 1; // H

      _update_wires(x0, y1, x0, y0);
      _wire_length += abs(y1-y0);  
      _direction[_used_num_wires] = 0; // V
    } else if (x1 < x0 && y1 > y0) { // case 3
      _update_wires(x1, y0, x1, y1);
      _wire_length += abs(x1-x0);
      _direction[_used_num_wires] = 0; // V

      _update_wires(x1, y0, x0, y0);
      _wire_length += abs(y1-y0);    
      _direction[_used_num_wires] = 1; // H
    } else {
      printf("%ld, %ld, %ld, %ld\n", x0, x1, y0, y1);
      printf("ERROR\n");
      exit(1);
    }
  }
}

void ST::Simulator::run() {
  // init
  _used_num_wires = 0;
  _wire_length = 0;
  _direction.resize(2*_num_pins, 0);
  _wires.resize(2*_num_pins);
  
  // connection
  for (size_t i = 1; i < _num_pins; i++) {
    size_t pre_pin_id = (i-1);
    size_t now_pin_id = i;
    if (_pins[pre_pin_id][0] == _pins[now_pin_id][0]) { // two pins have same x coordinate 
      size_t num_add_wires = 1;
      _assign_wires(num_add_wires, pre_pin_id, now_pin_id);
    } else if (_pins[pre_pin_id][1] == _pins[now_pin_id][1]) { // two pins have same y coordinate 
      size_t num_add_wires = 1;
      _assign_wires(num_add_wires, pre_pin_id, now_pin_id);      
    } else { // two pins have no same coordinate 
      size_t num_add_wires = 2;
      _assign_wires(num_add_wires, pre_pin_id, now_pin_id);      
    }
  }
}

void ST::Simulator::output_results(std::ofstream &output_file) {
  output_file << "NumRoutedPins = " << _num_pins << std::endl;
  output_file << "WireLength = " << _wire_length << std::endl;
  for (size_t i = 0; i < _used_num_wires; i++) {
    if (!_direction[i]) { // V-Line
      output_file << "V-line (" << _wires[i][0] << ", " << _wires[i][1] << ") " 
      << "(" << _wires[i][2] << ", " << _wires[i][3] << ") " << std::endl;
    } else { // H-Line
      output_file << "H-line (" << _wires[i][0] << ", " << _wires[i][1] << ") " 
      << "(" << _wires[i][2] << ", " << _wires[i][3] << ") " << std::endl;
    }
  }
}

