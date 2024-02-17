#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "./fiduccia_mattheyses_algorithm.hpp"

#define gain_to_gain_bucket_position(P_max, val) (P_max-val)
#define gain_bucket_position_to_gain(P_max, pos) (P_max-pos)

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

bool FM::Simulator::_check_balanced(size_t par_0_sz, size_t par_1_sz) {
  /**
   * size_CG*: n: total cell number
   * rule 1: (n*(1-_r)/2 <= size_G1)
   * rule 2: size_G2 <= n*(1+_r)/2
  */
  std::cout << "balanced factors: " << _balanced_factor_0 << ", " << _balanced_factor_1 << "\n";
  if ((_balanced_factor_0 <= par_0_sz) && (par_1_sz <= _balanced_factor_1)) {
    return true;
  }
  return false;
}

void FM::Simulator::_init_partitions() {
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
  if (!_check_balanced(_partitoin_0_sz, _partitoin_1_sz)) {
    std::cout << "init not balanced\n";
    exit(1);
  }
 
  _partition_counts.resize(2);
  _partition_counts[0].resize(_nets.size());
  _partition_counts[1].resize(_nets.size());
  for (size_t i = 0; i < _nets.size(); i++) {
    for (size_t j = 0; j < _nets[i].size(); j++) {
      if (_partition[_nets[i][j]] == 0) {
        _partition_counts[0][i]++;
      } else {
        _partition_counts[1][i]++;
      }
    }
  }
}

void FM::Simulator::_compute_gains(size_t idx, std::vector<size_t> cell) {
  // init gain, gain = FS(s)-TE(s)
  // where the “moving force“ FS(c) is the number of nets connected to c 
  // but not connected to any other cells within c’s partition, 
  // i.e., cut nets that connect 1 only to c, and
  // the “retention force“ TE(c) is the number of uncut nets connected to c.
  
  size_t FS = 0, TE = 0;
  bool which_par = _partition[idx];
  // compute FS  
  for (size_t i = 0; i < cell.size(); i++) {
    size_t which_net = cell[i];
    bool is_FS = true;
    for (size_t j = 0; j < _nets[which_net].size(); j++) { // j: cell index connected by this which_net
      size_t which_cell = _nets[which_net][j];
      bool j_cell_par = _partition[which_cell];
      if ((j_cell_par == which_par) && (which_cell != idx)) { // if check to myself -> skip
        is_FS = false;
        break;
      }
    }
    if (is_FS) {
      FS++;
    }
  }

  // compute TE
  for (size_t i = 0; i < cell.size(); i++) {
    size_t which_net = cell[i];
    bool is_TE = true;
    for (size_t j = 0; j < _nets[which_net].size(); j++) { // j: cell index connected by this which_net
      size_t which_cell = _nets[which_net][j];
      bool j_cell_par = _partition[which_cell];
      if ((j_cell_par != which_par)) { // the nets connected to the cells in different partitions
        is_TE = false;
        break;
      }    
    }
    if (is_TE) {
      TE++;
    }
  }

  _FS[idx] = FS;
  _TE[idx] = TE;
}

void FM::Simulator::_init_gains_and_gain_based_bucket() {
  // ask memory 
  _FS.resize(_cells.size());
  _TE.resize(_cells.size());
  
  // init gains
  for (size_t i = 0; i < _cells.size(); i++) {
    _compute_gains(i, _cells[i]);
  }
  
  // update _theory_P_max
  _theory_P_max = _nets.size()-0; // _theory_P_max = _theory_FS_max - _theory_TE_min
  _max_gain_pointer = (_theory_P_max<<1); // the smallest gain position in theory
  // ask memory 
  _gain_based_bucket.resize((_theory_P_max<<1)+1);

  // init bucket
  for (size_t i = 0; i < _cells.size(); i++) {
    // size_t cell_idx = i;
    size_t gain = _FS[i] - _TE[i];
    size_t pos = gain_to_gain_bucket_position(_theory_P_max, gain);
    // printf("i = %ld, gain = %ld, pos = %ld\n", i, gain, pos);
    _gain_based_bucket[pos].push_back(i);
    
    // init: _max_gain_pointer, point on the max gain position
    _max_gain_pointer = (pos < _max_gain_pointer) ? (pos) : (_max_gain_pointer);
  }
}

void FM::Simulator::_init() {
  // init partitoins
  _init_partitions();
#ifdef debug_read_input_file
  print_partitions();
#endif 

  
  // init gains and gain-based bucket
#ifdef debug_read_input_file
  printf("_init_gains_and_gain_based_bucket\n");
#endif
  
  _init_gains_and_gain_based_bucket();

#ifdef debug_read_input_file
  print_FS_TE_gain();
  print_init_gain_based_bucket();
#endif
}

void FM::Simulator::_update_partitions(size_t idx) {
  // update _partitions
  for (size_t i = 0; i < _cells[idx].size(); i++) {
    size_t net_idx = _cells[idx][i];
    _partition_counts[_partition[idx]][net_idx]--;
    _partition_counts[1 - _partition[idx]][net_idx]++;
  }
  _partition[idx] = 1 - _partition[idx];
}

void FM::Simulator::_update_FS_TE(size_t idx) {
  for (size_t i = 0; i < _cells[idx].size(); i++) {
    size_t net_idx = _cells[idx][i];
    for (size_t j = 0; j < _nets[net_idx].size(); j++) {
      size_t neighbor = _nets[net_idx][j];
      size_t n_par = _partition[neighbor], t_par = _partition[idx];

      if (neighbor != idx) {
        // we first handle the case where neighbor is not the target cell
        if (_partition_counts[n_par][net_idx] == 2 && n_par == t_par) {
          // after the update, neighbor will become the only cell in this partition
          _FS[neighbor]++;
        } else if (_partition_counts[n_par][net_idx] == 1) {
          // neighbor now is the only cell in this partition, but won't be anymore after the update
          _FS[neighbor]--;
        }
      }
      
      if (_partition_counts[t_par][net_idx] == 1) {
        // after the update, this net becomes uncut as every cell is in the same partition
        _TE[neighbor]++;
        if (neighbor == idx) {
          // handle the case where the neighbor is the target call
          _FS[idx]--;
        }
      } else if (_partition_counts[1 - t_par][net_idx] == 0) {
        // now every cell is in the same partition, but won't be anymore after the update
        _TE[neighbor]--;
        if (neighbor == idx) {
          // handle the case where the neighbor is the target call
          _FS[idx]++;
        }
      }
    }
  }
}

void FM::Simulator::_update_gain_pos_pointer() {
  for(size_t i = _max_gain_pointer; i < _gain_based_bucket.size(); i++) {
    if (_gain_based_bucket[_max_gain_pointer].size() != 0) {
      _max_gain_pointer = i;
      break;
    }
  }
}

void FM::Simulator::_update_gain_based_bucket() { 

}

void FM::Simulator::_update_cut_size () {

}

bool FM::Simulator::_improvement() {
  bool ret = true;

  return ret;
}

void FM::Simulator::run() {
  bool improvement = true; // has improvement 
  while (improvement) { // round of PASS
    // While there is unlocked object
    // 1. Each object is assigned a gain
    // 2. Objects are put into a sorted gain list
    // 3. The object with the highest gain from the larger of the two sides is selected and moved
    // 4. The moved object is "locked"
    // 5. Gains of "touched" objects are recomputed 
    // 6. Gain lists are resorted
    std::vector<size_t> locked; // 0: unlocked, 1: locked
    size_t locked_cells = 0;
    locked.resize(_cells.size()); // init = 0
    while(locked_cells < _cells.size()) {
      /**
       * step 3
       * select the cell with the largest gain
       * check if the moving of the cell still satisfy the balanced rule
       * if true
       * move
       * else
       * select the next one
       * */ 
      bool selected = false;
      bool belong_par;
      size_t par_0_sz, par_1_sz;
      
      size_t gain_pos_pointer = _max_gain_pointer;
      auto cell_index = _gain_based_bucket[gain_pos_pointer].begin();
      size_t selected_cell_idx = *cell_index;
      
      while (!selected) {
        belong_par = _partition[selected_cell_idx];
        par_0_sz = _partitoin_0_sz;
        par_1_sz = _partitoin_1_sz;
        if (belong_par) {
          par_0_sz++; // move from 1 -> 0
          par_1_sz--;  
        } else {
          par_0_sz--; // move from 0 -> 1
          par_1_sz++;        
        }

        bool balanced = _check_balanced(par_0_sz, par_1_sz);
        if (balanced && !locked[selected_cell_idx]) { // it is balanced and haven't locked
          selected = true;
        } else {
          if (cell_index != _gain_based_bucket[gain_pos_pointer].end()) {
            cell_index++;
          } else {
            // chose another line of bucket
            for (size_t i = gain_pos_pointer; i < _gain_based_bucket.size(); i++) {
              if (_gain_based_bucket[i].size() != 0) {
                cell_index = _gain_based_bucket[i].begin();
                gain_pos_pointer = i;
                break;
              }
            }
          }
        }
      }
      
      /**
       * step 4
       * 4. The moved object is "locked"
      */
      locked_cells++;
      locked[selected_cell_idx] = 1;

      std::cout << "selected_cell_idx: " << selected_cell_idx << std::endl;
      _update_FS_TE(selected_cell_idx);

      /**
       * step 5, 6
       * 5. Gains of "touched" objects are recomputed 
       * 6. Gain lists are resorted
      */
      /**
       * update: 
       * gain_pos_pointer
       * _gain_based_bucket
       * _partitoin_0_sz
       * _partitoin_1_sz
       * _partitions
       * _cut_size
       * _max_gain_pointer
       * */
      // update gain_pos_pointer
      _update_gain_pos_pointer();
      // update _partitoin_0_sz, _partitoin_1_sz
      _partitoin_0_sz = par_0_sz;
      _partitoin_1_sz = par_1_sz;
      _update_partitions(selected_cell_idx);

      // debug
      print_FS_TE_gain();
      print_partitions();
      exit(1);

      // todo: update _gain_based_bucket
      _update_gain_based_bucket();
      // todo: update _cut_size
      _update_cut_size();
    }


    // todo: update improvement
    improvement = _improvement();
    // record the partition results of this PASS
    _record_pass_results();

  }
}
void FM::Simulator::_record_pass_results(){
  // record partition results
  _partition_results_in_each_pass.push_back(_partition);

  // record cut size
  _cut_sizes_in_each_pass.push_back(_cut_size);
}

void FM::Simulator::get_results() {
 //todo: note the index of cells and nets should ADD by 1
 printf("Cutsize = %ld", _cut_size);
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

void FM::Simulator::print_partitions() {
  // partitions
  std::cout << "Partitions:\n";
  for (size_t i = 0; i < _cells.size(); i++) {
    std::cout << "cell_" << i << ", "<<  _partition[i] << "\n";
  }
  for (size_t i = 0; i < _nets.size(); i++) {
    std::cout << "NET n" << i << " ";
    std::cout << "partition 0, 1 count: (" << _partition_counts[0][i] << ", " << _partition_counts[1][i] << ")\n";
  }
}

void FM::Simulator::print_init_gain_based_bucket() {
  printf("_theory_P_max = %ld\n", _theory_P_max);
  for (size_t i = 0; i < _gain_based_bucket.size(); i++) {
    size_t gain = gain_bucket_position_to_gain(_theory_P_max, i);
    printf("Gain_val = %ld, includes cells: ", gain);
    for (auto it = _gain_based_bucket[i].begin(); it != _gain_based_bucket[i].end(); ++it) {
        printf("%ld, ", *it);
    } 
    printf("\n");
  }
  printf("_max_gain_pointer = %ld\n", _max_gain_pointer);
}

void FM::Simulator::print_FS_TE_gain() {
  for (size_t i = 0; i < _cells.size(); i++) {
    printf("idx = %ld, FS = %ld, TE = %ld, gain = %ld\n", i, _FS[i], _TE[i], _FS[i] - _TE[i]);
  }
}