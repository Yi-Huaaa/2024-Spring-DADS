#ifndef FM_ALGO_H
#define FM_ALGO_H

#include <iostream>
#include <list>
#include <vector>

// define for debugging
#define debug_read_input_file


// fsim namespace
namespace FM {


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
  std::vector<std::vector<bool>> _partition_results_in_each_pass;
  std::vector<size_t> _cut_sizes_in_each_pass;

  std::vector<std::vector<size_t>> _partition_counts; // Given a net, the number of cells in partition 0 and 1
  std::vector<size_t> _FS; // FS of each cell
  std::vector<size_t> _TE; // TE of each cell
  
  // vars
  double _r; // balance_factor
  double _balanced_factor_0;
  double _balanced_factor_1;
  size_t _partitoin_0_sz; // size of partition 0 (number of cells)
  size_t _partitoin_1_sz; // size of partition 1 (number of cells)
  size_t _theory_P_max; // in theory max FS
  size_t _max_gain_pointer; // point on the real max of gain
  size_t _cut_size;
  

  // functions 
  bool _check_balanced(size_t par_0_sz, size_t par_1_sz);
  void _init();
  void _init_partitions();
  void _init_gains_and_gain_based_bucket();
  void _compute_gains (size_t idx, std::vector<size_t> cell);
  bool _improvement();
  void _update_partitions(size_t idx);
  void _update_FS_TE(size_t idx);
  void _update_gain_pos_pointer();
  void _update_gain_based_bucket ();
  void _update_cut_size();
  void _record_pass_results();

  // print functions for debug
  void print_input_file();
  void print_partitions();
  void print_init_gain_based_bucket();
  void print_FS_TE_gain();
};

  

} // namespace FM




#endif