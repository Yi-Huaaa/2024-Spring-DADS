#ifndef FP_ALGO_H
#define FP_ALGO_H

#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>


// define for check 
// #define FP_READ_CEHCK
// #define FP_INIT_CEHCK

// fsim namespace
namespace FP {

class Simulator {

  // const size_t SIZE_T_MAX = ;


public:
  void read(std::ifstream &input_file_1, std::ifstream &input_file_2);
  void run(double alpha);
  void output_results(std::ofstream &output_file);

private:
  void _init(double alpha);
  void _SA();
  void _movement();
  void _PE_swap(size_t idx1, size_t idx2);
  void _PE_complement_chain(size_t idx);
  void _PE_block_rotate(size_t idx);
  bool _PE_validation(); // check whether the PE is valid
  void _change_PE_back();
  size_t _is_blk_or_ter(std::string input_str); // 0: blk, 1: ter

  std::unordered_map<std::string, size_t> _map_blk_to_id; // map block to id
  std::unordered_map<std::string, size_t> _map_ter_to_id; // map terminal to id
  size_t _constraint_width;
  size_t _constraint_height;
  size_t _num_blocks;
  size_t _num_terminals;
  std::vector<std::vector<size_t>> _blocks_w_h;
  std::vector<std::vector<size_t>> _terminals_w_h;

  size_t _num_nets;
  size_t _net_degree;
  std::vector<std::vector<size_t>> _nets; 

  std::vector<int> PE; // polish expression , -1: V, -2: H
  std::vector<size_t> _block_PE_idx; // record each block located at PE's which place

  size_t _chosen_movement; // choose which movement 
  size_t pick_op_0; // picked operator or operand_0 (index in PE)
  size_t pick_op_1; // picked operator or operand_1 (index in PE)
  double _alpha; // user defined ratio

  // variables for calculating area and length norm
  size_t _norm_count;
  size_t _norm_total;
  double _area_norm;
  double _length_norm;

  double _cur_cost;
  double _cur_length;
  size_t _cur_w;
  size_t _cur_h;
  std::vector<std::pair<double, double>> _cur_centers;

  double _best_cost;
  double _best_length;
  size_t _best_w;
  size_t _best_h;
  std::vector<std::pair<size_t, size_t>> _best_lb;
  std::vector<std::pair<size_t, size_t>> _best_ru;
  std::vector<int> _best_PE;

  void _compute_cost();
  void _update_best();
  std::pair<size_t, size_t> _compute_area(size_t start, size_t end, size_t x, size_t y);
  double _compute_length();

  // debug
  void _print_PE();
  size_t _area_sum;

  bool _found_in_bound;
  double penalty_factor = 2;

  bool _best_fit_init(bool cut_same_direction);
  bool early_break_find_better_or_not;
};

} // namespace FP

#endif