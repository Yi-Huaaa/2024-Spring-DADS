#ifndef FM_ALGO_H
#define FM_ALGO_H

#include <iostream>
#include <list>
#include <vector>

// fsim namespace
namespace FM {

class Simulator {

public:
  void read(std::ifstream &input_file);
  void run();
  void output_results(std::ofstream &output_file);

private:
  // should be constant all the time
  std::vector<std::vector<size_t>> _nets; // for each net, it connects to which cells
  std::vector<std::vector<size_t>> _cells; // for each cell, it connects to which nets

  // changes in each update
  std::vector<bool> _partition; // for each cell, it belongs to which partition
  std::vector<std::vector<size_t>> _partition_counts; // Given a net, the number of cells in partition 0 and 1
  std::vector<size_t> _FS; // FS of each cell
  std::vector<size_t> _TE; // TE of each cell
  std::vector<size_t> _gain; // gain of each cell (gain = FS - TE)
  std::vector<std::list<size_t>> _gain_based_bucket;
  std::vector<std::list<size_t>::iterator> _cell_iterators;
  std::vector<bool> _locked;
  
  // for best solution
  std::size_t _min_cut_size;
  std::size_t _cur_cut_size;
  std::vector<bool> _best_partition;
  
  // vars
  // should be constant all the time
  double _r; // balance_factor
  double _balanced_factor_0;
  double _balanced_factor_1;
  size_t _theory_P_max; // in theory max FS

  // changes in each update
  size_t _partitoin_0_sz; // size of partition 0 (number of cells)
  size_t _partitoin_1_sz; // size of partition 1 (number of cells)
  size_t _max_gain_pointer; // point on the real max of gain

  // functions 
  bool _check_balanced(const size_t par_0_sz, const size_t par_1_sz);
  void _init();
  void _init_partitions();
  void _init_gains_and_gain_based_bucket();
  void _compute_gains (const size_t idx, std::vector<size_t> &cell);
  void _update_partitions(const size_t idx);
  void _update_FS_TE_gain(const size_t idx);
  void _update_cut_size();

  // print functions for debug
  void print_input_file();
  void print_partitions();
  void print_gain_based_bucket();
  void print_FS_TE_gain();

  // help to deal with 1-based and 0-based
  size_t _which_based;
  void _prune_fake_data();
  size_t _max_num_cells;
  std::vector<bool> _real_data;
};

  

} // namespace FM




#endif