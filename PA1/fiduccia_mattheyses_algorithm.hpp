#ifndef FM_ALGO_H
#define FM_ALGO_H

#include <iostream>
#include <list>
#include <vector>

// define for debugging
#define debug_read_input_file


// fsim namespace
namespace FM {

// typedef struct NET {
//   size_t idx;
    
// } NET;
 
// typedef struct CELL {
//   size_t idx;
//   std::vector<size_t> nets;


    
// } CELL;


class Simulator {

public:
  void read(std::ifstream &input_file);
  void run();
  void get_results();

private: 
  std::vector<std::vector<size_t>> _nets; // for each net, it connects to which cells
  std::vector<std::vector<size_t>> _cells; // for each cell, it connects to which nets 
  std::vector<bool> _partition; // for each cell, it belongs to which partition
  std::vector<std::list<size_t>> _gain_based_bucket;

  double _r; // balance_factor
  double _balanced_factor_0;
  double _balanced_factor_1;
  size_t _partitoin_0_sz; // size of partition 0 (number of cells)
  size_t _partitoin_1_sz; // size of partition 1 (number of cells)

  // functions 
  bool _check_balanced();
  void _init();
  void _init_partitions();
  void _init_gains();
  void _compute_gains (std::list<size_t> cell);
  void _update_gains ();
  void _init_gain_based_bucket();


  // print functions for debug
  void print_input_file();
  void print_init ();
};

  

} // namespace FM




#endif