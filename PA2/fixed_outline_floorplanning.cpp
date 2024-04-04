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

#include "./fixed_outline_floorplanning.hpp"

#define FP_ITERATION 1

// public functions
void FP::Simulator::read(std::ifstream &input_file_1, std::ifstream &input_file_2) {
  // input file 1
  std::string tmp;
  input_file_1 >> tmp >> _constraint_width >> _constraint_height;
  input_file_1 >> tmp >> _num_blocks;
  input_file_1 >> tmp >> _num_terminals; 

  _blocks_w_h.resize(_num_blocks);
  _terminals_w_h.resize(_num_terminals);

  // read blocks
  for (size_t i = 0; i < _num_blocks; i++) {
    _blocks_w_h[i].resize(2);
    size_t width, height;
    std::string blk_name;
    input_file_1 >> blk_name >> width >> height;
    _blocks_w_h[i][0] = width;
    _blocks_w_h[i][1] = height;
    _map_blk_to_id[blk_name] = i;
  }

  // read terminals
  for (size_t i = 0; i < _num_terminals; i++) {
    _terminals_w_h[i].resize(2);
    size_t x, y;
    std::string ter_name;
    input_file_1 >> ter_name >> tmp >> x >> y;
    _terminals_w_h[i][0] = x;
    _terminals_w_h[i][1] = y;
    _map_ter_to_id[ter_name] = _num_blocks + i;
  }

  // input file 2
  input_file_2 >> tmp >> _num_nets;
  _nets.resize(_num_nets);

  for (size_t i = 0; i < _num_nets; i++) {
    input_file_2 >> tmp >> _net_degree;
    _nets[i].resize(_net_degree);
    for (size_t j = 0; j < _net_degree; j++) {
      input_file_2 >> tmp;
      size_t b_o_t = _is_blk_or_ter(tmp); // is block (0) or terminal (1)
      _nets[i][j] = (b_o_t) ? (_map_ter_to_id[tmp]) : (_map_blk_to_id[tmp]);
    }
  }

#ifdef FP_READ_CEHCK  
  // file 1
  printf("_constraint_width = %zu, _constraint_height = %zu\n_num_blocks = %zu, _num_terminals = %zu\n", _constraint_width, _constraint_height, _num_blocks, _num_terminals);
  
  printf("_blocks_w_h:\n");
  for (size_t i = 0; i < _num_blocks; i++) {
    printf("bk_%zu %zu %zu\n", (i+1), _blocks_w_h[i][0], _blocks_w_h[i][1]);
  } printf("\n");
#endif
}

std::vector<size_t> sort_indices(const std::vector<std::vector<size_t>>& vec, size_t key_index) {
  std::vector<size_t> indices(vec.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(), [&](size_t i, size_t j) {
    return vec[i][key_index] > vec[j][key_index];
  });
  return indices;
}

std::vector<size_t> sort_indices2(const std::vector<std::vector<size_t>>& vec, size_t key_index, size_t start, size_t end) {
  std::vector<size_t> indices(end-start);
  std::iota(indices.begin() + start, indices.begin() + end, 0);
  std::sort(indices.begin(), indices.end(), [&](size_t i, size_t j) {
    return vec[i][key_index] > vec[j][key_index];
  });
  return indices;
}

void FP::Simulator::_init(double alpha) {  
  // set a random seed
  // srand(time(NULL));
  srand(9487);
  _alpha = alpha;

  // set initial PE and the start point of blocks
  // init: set all into vertical 
  int vertical = -1;
  size_t push_back_vectrical = 0;
  // postorder traversal: 
  for (size_t i = 0; i < _num_blocks; i += 2) {
    PE.push_back(i);
    if ((i + 1) < _num_blocks) PE.push_back(i + 1);
    PE.push_back(vertical);
    vertical = -3 - vertical; // switch between V and H
    push_back_vectrical++;
  }

  // pad H/V at the end of PE
  push_back_vectrical -= (_num_blocks & 1); // _num_blocks == even, -1, else -2
  push_back_vectrical--;
  for (size_t i = 0; i < push_back_vectrical; i++) {
    PE.push_back(vertical);
    vertical = -3 - vertical; // switch between V and H
  }

  // try all kinds of init
  if (_best_fit_init(true)) {
    // std::cout << "_best_fit_init(true) success!" << std::endl;
  } else if (_best_fit_init(false)) {
    // std::cout << "_best_fit_init(false) success!" << std::endl;
  } else {
    // std::cout << "all init failed!" << std::endl;
  }

#ifdef FP_INIT_CEHCK
  printf("PE:\n");
  _print_PE();
#endif

  // init norm variables
  _norm_count = 0;
  _norm_total = 2 * _num_blocks; // TODO: adjust this
  _area_norm = 0;
  _length_norm = 0;

  // compute the cost of the initial PE
  _cur_centers.resize(_num_blocks);
  _best_lb.resize(_num_blocks);
  _best_ru.resize(_num_blocks);
  _compute_cost();
  _update_best();

  // calculate ideal area and ratio
  _area_sum = 0;
  for (size_t i = 0; i < _num_blocks; i++) {
    _area_sum += _blocks_w_h[i][0] * _blocks_w_h[i][1];
  }
  // std::cout << "area_sum = " << _area_sum << std::endl;
  // std::cout << "bound = " << _constraint_width * _constraint_height << std::endl;
  // std::cout << "ratio = " << double(_area_sum) / (_constraint_width * _constraint_height) << std::endl;
}

std::vector<int> reconstruct(
  std::unordered_map<int, std::pair<int, int>> &children,
  std::unordered_map<int, int> &prev_cuts,
  int root
) {
  std::vector<int> result;
  result.push_back(root);
  if (children[root].second != -1) {
    std::vector<int> second = reconstruct(children, prev_cuts, children[root].second);
    result.insert(result.end(), second.begin(), second.end());
    result.push_back(-3 - prev_cuts[root]); // the inverse
  }
  if (children[root].first != -1) {
    std::vector<int> first = reconstruct(children, prev_cuts, children[root].first);
    result.insert(result.end(), first.begin(), first.end());
    result.push_back(prev_cuts[root]); // the same
  }
  return result;
}

bool FP::Simulator::_best_fit_init(bool cut_same_direction) {
  // init
  std::vector<size_t> record_base_x; record_base_x.resize(_num_blocks);
  std::vector<size_t> record_base_y; record_base_y.resize(_num_blocks);
  std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, int, int>> spaces;
  std::vector<size_t> candi;
  std::unordered_map<int, std::pair<int, int>> children;
  std::unordered_map<int, int> prev_cuts;

  int dummy_id = 999;
  spaces.push_back(std::make_tuple(0, 0, _constraint_width, _constraint_height, dummy_id, -1, -1));
  for (size_t i = 0; i < _num_blocks; i++) {
    candi.push_back(i);
    children[i] = std::make_pair(-1, -1);
  }
  children[dummy_id] = std::make_pair(-1, -1);
  prev_cuts[dummy_id] = -1; // V

  // find best
  for (size_t a = 0; a < _num_blocks; a++) {
    size_t best_waste = 1000000000000, best_i, best_j, best_rot, best_cut;
    for (size_t i = 0; i < spaces.size(); i++) {
      auto [base_x, base_y, space_w, space_h, prev_block_id, prev_cut, rel_cut] = spaces[i];
      for (size_t j = 0; j < candi.size(); j++) {
        size_t block_id = candi[j];
        size_t block_w = _blocks_w_h[block_id][0], block_h = _blocks_w_h[block_id][1];
        size_t waste;
        if (space_w >= block_w && space_h >= block_h) {
          waste = space_w - block_w; // cut V
          if (waste < best_waste) {
            best_waste = waste;
            best_i = i;
            best_j = j;
            best_rot = 0;
            best_cut = cut_same_direction ? 0 : 1;
          }
          waste = space_h - block_h; // cut H
          if (waste < best_waste) {
            best_waste = waste;
            best_i = i;
            best_j = j;
            best_rot = 0;
            best_cut = cut_same_direction ? 1 : 0;
          }
        }
        if (space_w >= block_h && space_h >= block_w) {
          waste = space_w - block_h; // rotate and cut V
          if (waste < best_waste) {
            best_waste = waste;
            best_i = i;
            best_j = j;
            best_rot = 1;
            best_cut = cut_same_direction ? 0 : 1;
          }
          waste = space_h - block_w; // rotate and cut H
          if (waste < best_waste) {
            best_waste = waste;
            best_i = i;
            best_j = j;
            best_rot = 1;
            best_cut = cut_same_direction ? 1 : 0;
          }
        }       
      }   
    }
    if (best_waste == 1000000000000) {
      // std::cout << "NOT FIT!\n";
      return false;
    }

    // split
    size_t best_block_id = candi[best_j];
    if (best_rot == 1) {
      size_t tmp = _blocks_w_h[best_block_id][0];
      _blocks_w_h[best_block_id][0] = _blocks_w_h[best_block_id][1];
      _blocks_w_h[best_block_id][1] = tmp;
    }
    size_t block_w = _blocks_w_h[best_block_id][0], block_h = _blocks_w_h[best_block_id][1];

    auto [base_x, base_y, space_w, space_h, prev_block_id, prev_cut, rel_cut] = spaces[best_i];
    std::tuple<size_t, size_t, size_t, size_t, size_t, int, int> space1, space2;
    if (best_cut == 0) {
      // cut V
      space1 = std::make_tuple(base_x, base_y + block_h, block_w, space_h - block_h, best_block_id, -best_cut - 1, -2);
      space2 = std::make_tuple(base_x + block_w, base_y, space_w - block_w, space_h, best_block_id, -best_cut - 1, -1);
    } else {
      // cut H
      space1 = std::make_tuple(base_x, base_y + block_h, space_w, space_h - block_h, best_block_id, -best_cut - 1, -2);
      space2 = std::make_tuple(base_x + block_w, base_y, space_w - block_w, block_h, best_block_id, -best_cut - 1, -1);
    }
    
    spaces.erase(spaces.begin() + best_i);
    spaces.push_back(space2);
    spaces.push_back(space1);
    candi.erase(candi.begin() + best_j);

    // std::cout << "========================\n";
    // std::cout << "picked space: (base_x, base_y) = (" << base_x << ", " << base_y << "), (space_w, space_h) = (" << space_w << ", " << space_h << ")\n";
    // std::cout << "picked block: id = " << best_block_id << ", (w, h) = (" << block_w << ", " << block_h << ")" << std::endl;
    // std::cout << "prev: id = " << prev_block_id << ", prev_cut = " << prev_cut << ", rel_cut = " << rel_cut << std::endl;

    if (prev_cut == rel_cut) {
      children[prev_block_id].first = best_block_id;
    } else {
      children[prev_block_id].second = best_block_id;
    }
    prev_cuts[prev_block_id] = prev_cut;
  }

  std::vector<int> result = reconstruct(children, prev_cuts, dummy_id);
  result.erase(result.begin());
  result.erase(result.end() - 1);
  PE = result;
  // std::cout << "SUCCESS!\n";
  return true;
}

void FP::Simulator::run(double alpha) {
  // initialization
  _init(alpha);

  // iteration
  for (size_t iter = 0; iter < FP_ITERATION; iter++) {
    _SA();
  }
}

void FP::Simulator::output_results(std::ofstream &output_file, double duration) {
  output_file << _best_cost << std::endl;
  output_file << size_t(_best_length) << std::endl;
  output_file << _best_w * _best_h << std::endl;
  output_file << _best_w << " " << _best_h << std::endl;
  output_file << duration << std::endl;
  for (const auto& pair: _map_blk_to_id) {
    auto& [block_name, block_id] = pair;
    output_file << pair.first << " ";
    output_file << _best_lb[block_id].first << " " << _best_lb[block_id].second << " ";
    output_file << _best_ru[block_id].first << " " << _best_ru[block_id].second << std::endl;
  }
}

size_t FP::Simulator::_is_blk_or_ter(std::string input_str) {
  if (_map_blk_to_id.find(input_str) == _map_blk_to_id.end()) {
    // we cannot find
    return 1;
  }
  return 0;
}

int random(int min, int max) {
  return std::rand() % (max - min + 1) + min;
}

bool FP::Simulator::_PE_validation() {
  int count = 0;
  for (int x: PE) {
    if (x >= 0) count += 1;
    else {
      if (count < 2) return false;
      count -= 1;
    }
  }
  return count == 1;
}

void FP::Simulator::_PE_swap(size_t idx1, size_t idx2) {
  int tmp = PE[idx1];
  PE[idx1] = PE[idx2];
  PE[idx2] = tmp;
}

void FP::Simulator::_PE_complement_chain(size_t idx) {
  for (size_t i = idx; i < PE.size(); i++) {
    if (PE[i] >= 0) break;
    PE[i] = -3 - PE[i]; // V <-> H
  }
}

void FP::Simulator::_PE_block_rotate(size_t idx) {
  size_t block_id = PE[idx];
  size_t tmp = _blocks_w_h[block_id][0];
  _blocks_w_h[block_id][0] = _blocks_w_h[block_id][1];
  _blocks_w_h[block_id][1] = tmp;
}

void FP::Simulator::_movement() {

  // bi
  size_t max_failures = 10;
  bool valid_movement = false;
  while (!valid_movement) {
    _chosen_movement = random(0, 3);
    if (_chosen_movement == 0) { // movement_0: swap adjacent numbers
      size_t failed_find_times = 0;
      while (failed_find_times < max_failures) {
        pick_op_0 = random(0, PE.size() - 2);
        if (PE[pick_op_0] >= 0) { // pick a number
          pick_op_1 = pick_op_0 + 1;          
          while (PE[pick_op_1] < 0 && pick_op_1 < PE.size()) { // find the adjacent number
            pick_op_1++;
          }
          if (PE[pick_op_1] >= 0 && pick_op_1 < PE.size()) {
            valid_movement = true; // find a valid movement_0
            break;
          }
        }
        failed_find_times++;
      }
      if (valid_movement) _PE_swap(pick_op_0, pick_op_1);
    } else if (_chosen_movement == 1) { // movement_1: complement chains
      while (true) {
        pick_op_0 = random(0, PE.size() - 1);
        if (PE[pick_op_0] < 0) {
          _PE_complement_chain(pick_op_0);
          valid_movement = true; // find a valid movement_1 
          break;
        }
      }
    } else if (_chosen_movement == 2) { // movement_2: swap adjacent number and symbol but need to check
      size_t failed_find_times = 0;
      while (failed_find_times < max_failures) {
        pick_op_0 = random(0, PE.size() - 2);
        pick_op_1 = pick_op_0 + 1;

        // symbol + number or number + symbol
        if ((PE[pick_op_0] < 0 && PE[pick_op_1] >= 0) || (PE[pick_op_0] >= 0 && PE[pick_op_1] < 0)) {
          _PE_swap(pick_op_0, pick_op_1);
          valid_movement = _PE_validation();
          if (valid_movement) {
            break;
          } else {
            _PE_swap(pick_op_0, pick_op_1);
          }
        }
        failed_find_times++;
      }
    } else if (_chosen_movement == 3) { // rotate
      do {
        pick_op_0 = random(0, PE.size() - 1);
      } while (PE[pick_op_0] < 0);
      _PE_block_rotate(pick_op_0);
      valid_movement = true;
    }
  }
  // ------------- debug message ------------- 
  // if (_chosen_movement == 0) {
  //   printf("DEBUGGG: Pick valid_moevement_%zu, pick_op_0 = %d (%d), pick_op_1 = %d (%d)\n", _chosen_movement, pick_op_0, PE[pick_op_0], pick_op_1, PE[pick_op_1]);
  // } else if (_chosen_movement == 1) {
  //   std::string a, b;
  //   if (PE[pick_op_0] == -1) {
  //     a = 'V';
  //   } else {
  //     a = 'H';
  //   }
  //   if (PE[pick_op_1] == -1) {
  //     b = 'V';
  //   } else {
  //     b = 'H';
  //   }
  //   printf("DEBUGGG: Pick valid_moevement_%zu, pick_op_0 = %d (%s), pick_op_1 = %d (%s)\n", _chosen_movement, pick_op_0, a.c_str(), pick_op_1, b.c_str());
  // } else { // _chosen_movement == 2
  //   std::string a, b;
  //   if (PE[pick_op_0] < 0) {
  //     if (PE[pick_op_0] == -1) {
  //       a = 'V';
  //     } else {
  //       a = 'H';
  //     }      
  //   } else {
  //     a = std::to_string(PE[pick_op_0]);
  //   }
  //   if (PE[pick_op_1] < 0) {
  //     if (PE[pick_op_1] == -1) {
  //       b = 'V';
  //     } else {
  //       b = 'H';
  //     }
  //   } else {
  //     b = std::to_string(PE[pick_op_1]);
  //   }
  //   printf("DEBUGGG: Pick valid_moevement_%zu, pick_op_0 = %d (%s), pick_op_1 = %d (%s)\n", _chosen_movement, pick_op_0, a.c_str(), pick_op_1, b.c_str());
  // }
  // ------------- debug message -------------
}

void FP::Simulator::_change_PE_back() {
  switch (_chosen_movement) {
    case 0: {
      _PE_swap(pick_op_0, pick_op_1);
      break;
    }
    case 1: {
      _PE_complement_chain(pick_op_0);
      break;
    }
    case 2: {
      _PE_swap(pick_op_0, pick_op_1);
      break;
    }
    case 3: {
      _PE_block_rotate(pick_op_0);
      break;
    }
    default: {
      // programming error
      exit(1);
    }
  }
}

void FP::Simulator::_print_PE() {
  for (int x: PE) {
    if (x == -1) std::cout << "V ";
    else if (x == -2) std::cout << "H ";
    else std::cout << x << " ";
  };
  std::cout << std::endl;
}

void FP::Simulator::_SA() {
  _found_in_bound = false;
  size_t tried_count = 0;
  double T = 1000;
  double T_threshold = 0.1; // 決定要跑幾 round
  double alpha = 0.95;
  size_t k = 30000;
  size_t max_tried_count = k * 3;
  _early_break_find_better_or_not = false;
  double early_break_T = 550;
  while (T > T_threshold) {
    // std::cout << "start new SA round: T = " << T << std::endl;
    for (size_t i = 0; i < k; i++) {
      tried_count += 1;
      // _print_PE();
      _movement();
      // _print_PE();

      double prev_cost = _cur_cost;
      _compute_cost();
      double delta_cost = _cur_cost - prev_cost;

      if (delta_cost < 0) {
        // better cost
      } else {
        double rand_val = double(rand()) / RAND_MAX;
        double prob = exp((-delta_cost) / T);
        if (T < 400.0) {
          prob /= 4;
        }
        if (prob > rand_val) {
          // worse cost, but accept this change
        } else {
          // if did not accpet the choice, need to change PE back
          // reverse all actions 
          _change_PE_back();
          _compute_cost();
        }
      }
    }

    if (_found_in_bound || tried_count >= max_tried_count) {
      // decrease the temp
      T *= alpha;
      _found_in_bound = false;
      tried_count = 0;
    } else {
      // didn't find in bound, don't decrease the temp
      _penalty_factor *= 2;
    }
    if (T < early_break_T && !_early_break_find_better_or_not) {
      return;
    }
  }
}

void FP::Simulator::_update_best() {
  _best_length = _cur_length;
  _best_w = _cur_w;
  _best_h = _cur_h;
  _best_cost = _cur_cost;
  for (size_t i = 0; i < _num_blocks; i++) {
    auto [center_x, center_y] = _cur_centers[i];
    size_t block_w = _blocks_w_h[i][0], block_h = _blocks_w_h[i][1];
    size_t lb_x = (size_t(2 * center_x) - block_w) / 2, ru_x = lb_x + block_w;
    size_t lb_y = (size_t(2 * center_y) - block_h) / 2, ru_y = lb_y + block_h;
    _best_lb[i].first = lb_x;
    _best_lb[i].second = lb_y;
    _best_ru[i].first = ru_x;
    _best_ru[i].second = ru_y;
    // printf("i = %ld, lb_x = %ld, lb_y = %ld\n", i, lb_x, lb_y);
    // std::cout << "center_x " << center_x << ", center_y " << center_y << "\n";
    // std::cout << "block_w " << block_w << ", block_h " << block_h << "\n";
  }
  _best_PE = PE;
  _early_break_find_better_or_not = true;
}

void FP::Simulator::_compute_cost() {
  auto [total_w, total_h] = _compute_area(0, PE.size(), 0, 0);
  size_t area = total_w * total_h;
  double length = _compute_length();

  // update norm if needed
  if (_norm_count < _norm_total) {
    // TODO: should we only include valid solutions?
    _area_norm = (_area_norm * _norm_count + area) / (_norm_count + 1);
    _length_norm = (_length_norm * _norm_count + length) / (_norm_count + 1);
    _norm_count++;
  }

  // set as current (for update best)
  _cur_length = length;
  _cur_w = total_w;
  _cur_h = total_h;

  // add penalty if out of bounds
  // double _penalty_factor = 2; // TODO: adjust this
  if (total_w > _constraint_width) {
    // area += _penalty_factor * (total_w - _constraint_width) * total_h;
    length += _penalty_factor * (total_w - _constraint_width);
  }
  if (total_h > _constraint_height) {
    // area += _penalty_factor * (total_h - _constraint_height) * total_w;
    length += _penalty_factor * (total_h - _constraint_height);
  }
  _cur_cost = _alpha * area / _area_norm + (1 - _alpha) * length / _length_norm;
  bool is_in_bound = total_w <= _constraint_width && total_h <= _constraint_height;
  
  // update found in bound for this temp
  if (is_in_bound) _found_in_bound = true;

  // check if can update best
  if (_cur_cost < _best_cost && is_in_bound) {
    // std::cout << "FOUND IN BOUND!!!\n";
    // std::cout << "update best cost: " << _best_cost << std::endl;
    _update_best();
  }
}

std::pair<size_t, size_t> FP::Simulator::_compute_area(size_t start, size_t end, size_t base_x, size_t base_y) {
  if (end == start + 1) {
    size_t block_id = PE[start];
    size_t block_w = _blocks_w_h[block_id][0], block_h = _blocks_w_h[block_id][1];

    // set the center points so we can compute lengths
    _cur_centers[block_id].first = base_x + double(block_w) / 2;
    _cur_centers[block_id].second = base_y + double(block_h) / 2;
    return std::make_pair(block_w, block_h);
  }

  // find the `i` that splits left and right trees
  int count = 1, i;
  for (i = int(end) - 1; i >= int(start); i--) {
    count += (PE[i] >= 0) ? 1 : -1;
    if (count == 1) break;
  }

  // compute area of left tree
  auto [left_w, left_h] = _compute_area(start, i, base_x, base_y);
  if (PE[end - 1] == -1) {
    // V case
    base_x += left_w;
  } else {
    // H case
    base_y += left_h;
  }

  // compute area of right tree
  auto [right_w, right_h] = _compute_area(i, end - 1, base_x, base_y);

  // return the total width and height
  if (PE[end - 1] == -1) {
    // V case
    return std::make_pair(left_w + right_w, std::max(left_h, right_h));
  } else {
    // H case
    return std::make_pair(std::max(left_w, right_w), left_h + right_h);
  }
}

double FP::Simulator::_compute_length() {
  double total_length = 0;
  for (const auto& ids: _nets) { // NOTE: _nets contain block or terminal IDs
    double min_x = 1000000000000, max_x = 0;
    double min_y = 1000000000000, max_y = 0;
    for (size_t id: ids) {
      double cur_x, cur_y;
      if (id < _num_blocks) {
        // this ID is for a block
        cur_x = _cur_centers[id].first;
        cur_y = _cur_centers[id].second;
      } else {
        // this ID is for a terminal
        cur_x = _terminals_w_h[id - _num_blocks][0];
        cur_y = _terminals_w_h[id - _num_blocks][1];
      }
      if (cur_x < min_x) {
        min_x = cur_x;
      }
      if (cur_x > max_x) {
        max_x = cur_x;
      }
      if (cur_y < min_y) {
        min_y = cur_y;
      }
      if (cur_y > max_y) {
        max_y = cur_y;
      }
    }

    // HPWL
    total_length += (max_x - min_x) + (max_y - min_y);
  }
  return total_length;
}
