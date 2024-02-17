#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "./fiduccia_mattheyses_algorithm.hpp"

// todo: printout the resutls need to be acreful of the index (both cells and nets)

// functions 
void FM::Simulator::read(std::ifstream &input_file) {
  size_t max_num_cells = 0; // max number of cells, local var is enough

  // start read file
  std::string line;
  std::getline(input_file, line);
  _r = std::stod(line);

  // construct _nets
  while (true) {
    std::string str;
    input_file >> str;
    
    if(input_file.eof()) {
      break;
    } else if (str == "NET") {
      input_file >> str;
      str = str.substr(1); // get net_idx
      // insert new vector
      _nets.push_back(std::vector<size_t>());
    }
    
    // read in the continuous cell numbers
    while (str != ";") {
      input_file >> str;

      if (str == ";") {
        break; // Exit loop if encountering ;
      }      

      std::string substring = str.substr(1);
      size_t cellNumber = std::stoul(substring)-1; // note: here minus 1 changed the index from 1-based to 0-based

      _nets[_nets.size()-1].push_back(cellNumber);
      // update max number of cells
      max_num_cells = (cellNumber > max_num_cells) ? (cellNumber) : (max_num_cells); 
    }
  }
  // update max_num_cells due to the based changed
  max_num_cells++; 

  // construct _cells
  _cells.resize(max_num_cells);
  
  for (size_t i = 0; i < _nets.size(); i++) {
    for (size_t j = 0; j < _nets[i].size(); j++) {
      size_t cell_idx = _nets[i][j];
      size_t net_idx = i;
      _cells[cell_idx].push_back(net_idx);
    }
  }

#ifdef debug_read_input_file
  std::cout << "read\n";
  std::cout.precision(20);
  std::cout << "_r =" << _r << "\n";
  std::cout << "max_num_cells = "<< max_num_cells << "\n\n";
  print_input_file();
#endif

  // update balanced_factors
  _balanced_factor_0 = (_cells.size()*(1-_r)/2);
  _balanced_factor_1 = (_cells.size()*(1+_r)/2);

  // initialize Gain
  _init();

}



bool FM::Simulator::_check_balanced() {
  /**
   * size_CG*: n: total cell number
   * rule 1: (n*(1-_r)/2 <= size_G1)
   * rule 2: size_G2 <= n*(1+_r)/2
  */
  std::cout << "balanced factors: " << _balanced_factor_0 << ", " << _balanced_factor_1 << "\n";
  if ((_balanced_factor_0 <= _partitoin_0_sz) && (_partitoin_1_sz <= _balanced_factor_1)) {
    return true;
  }
  return false;
}

void FM::Simulator::_init_partitions () {
  _partition.resize(_cells.size());
  _partitoin_0_sz = (_cells.size()>>1);
  _partitoin_1_sz = _cells.size()-_partitoin_0_sz;
  
  for (size_t i = 0; i < _partitoin_0_sz; i++) {
    _partition[i] = 0;
  }

  for (size_t i = _partitoin_0_sz; i < _cells.size(); i++) {
    _partition[i] = 1;
  }
  
  std::cout << "_partitoin_sz: " << _partitoin_0_sz << ", " << _partitoin_1_sz << "\n";

  // check the balanced
  if (!_check_balanced()) {
    std::cout << "init not balanced\n";
    exit(1);
  }
}

void FM::Simulator::_compute_gains (std::list<size_t> cell) {

}

void FM::Simulator::_init_gains () {
  // init gain, gain = FS(s)-TE(s)
  
}

void FM::Simulator::_update_gains () {
  
}


void FM::Simulator::_init_gain_based_bucket(){

}

void FM::Simulator::_init(){
  // init partitoins
  _init_partitions();

  // init gains
  _init_gains();

  // init gain-based bucket
  _init_gain_based_bucket();

#ifdef debug_read_input_file
  print_init();
#endif 
}




void FM::Simulator::run() {
  size_t improvement = 1; // has improvement 
  while (improvement) { // round of PASS
    // While there is unlocked object
    // 1. Each object is assigned a gain
    // 2. Objects are put into a sorted gain list
    // 3. The object with the highest gain from the larger of the two sides is selected and moved
    // 4. The moved object is "locked"
    // 5. Gains of "touched" objects are recomputed 6. Gain lists are resorted


    // todo: update improvement

  }
}

void FM::Simulator::get_results() {
 //todo: note the index of cells and nets should ADD by 1
}


// pprint functions for debugging 
void FM::Simulator::print_input_file () {
  std::cout << "PRINT _nets\n";
  for (size_t i = 0; i < _nets.size(); i++) {
    std::cout << "NET n" << (i) << ", w/ cells: ";
    for (size_t j = 0; j < _nets[i].size(); j++) {
      std::cout << _nets[i][j] << ", ";
    }
    std::cout << "\n";
  }
  std::cout << "PRINT _cells\n";
  for (size_t i = 0; i < _cells.size(); i++) {
    std::cout << "cell c" << (i) << ", connected by nets: ";
    for (size_t j = 0; j < _cells[i].size(); j++) {
      std::cout << _cells[i][j] << ", ";
    }
    std::cout << "\n";
  }  
}

void FM::Simulator::print_init () {
  // partitions
  std::cout << "_init_partitions\n";
  for (size_t i = 0; i < _cells.size(); i++) {
    std::cout << "cell_" << i << ", "<<  _partition[i] << "\n";
  }
  
  
  
  // 
}