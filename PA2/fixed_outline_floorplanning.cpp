#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <cstddef>

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


  for (size_t i = 0; i < _num_blocks; i++) {
    _blocks_w_h[i].resize(2);
    size_t width, height;
    std::string blk_name;
    input_file_1 >> blk_name >> width >> height;
    _blocks_w_h[i][0] = width;
    _blocks_w_h[i][1] = height;
    _map_blk_to_id[blk_name] = i;
  }

  for (size_t i = 0; i < _num_terminals; i++) {
    _terminals_w_h[i].resize(2);
    size_t x, y;
    std::string ter_name;
    input_file_1 >> ter_name >> tmp >> x >> y;
    _terminals_w_h[i][0] = x;
    _terminals_w_h[i][1] = y;
    _map_ter_to_id[ter_name] = i;
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

  for (size_t i = 0; i < _num_terminals; i++) {
    printf("terminal_%zu %zu %zu\n", i, _terminals_w_h[i][0], _terminals_w_h[i][1]);
  } printf("\n");

  // file 2
  for (size_t i = 0; i < _num_nets; i++) {
    printf("Net_%zu, _net_degree = %zu:\n", i, _nets[i].size());
    for (size_t j = 0; j < _nets[i].size(); j++) {
      printf("%zu\n", _nets[i][j]);
    }
  }
#endif
}

void FP::Simulator::_init(double alpha) {  
  // set a random seed
  srand(9487);

  _alpha = alpha;

  // set initial PE and the start point of blocks
  // init: set all into vertical 
  int vertical = -1;
  size_t push_back_vectrical = 0;
  // postorder traversal: 
  for (size_t i = 0; i < _num_blocks; i+=2) {
    PE.push_back(i);
    if ((i+1) < _num_blocks)
      PE.push_back(i+1);
    PE.push_back(vertical);
    push_back_vectrical++;
  }

  printf("pre: push_back_vectrical = %ld\n", push_back_vectrical);

  // size_t a = ((_num_blocks)&1 == 0) ? ((push_back_vectrical-1)) : ((push_back_vectrical-2)); // 
  push_back_vectrical -= (_num_blocks & 1); // _num_blocks == even, -1, else -2
  push_back_vectrical--;
  printf("post: push_back_vectrical = %ld\n", push_back_vectrical);
  for (size_t i = 0; i < push_back_vectrical; i++) {
    PE.push_back(vertical);
  }

#ifdef FP_INIT_CEHCK
  printf("PE:\n");
  _print_PE();
#endif

  // init norm variables
  _norm_count = 0;
  _norm_total = 10; // TODO: adjust this based on prob size
  _area_norm = 0;
  _length_norm = 0;

  // compute the cost of the initial PE
  _cur_centers.resize(_num_blocks);
  _compute_cost();
  _update_best();
}

void FP::Simulator::run(double alpha) {
  // initialization
  _init(alpha);

  // iteration
  for (size_t iter = 0; iter < FP_ITERATION; iter++) {
    _SA();
  }
}

void FP::Simulator::output_results(std::ofstream &output_file) {
  output_file << _best_cost << std::endl;
  output_file << _best_length << std::endl;
  output_file << _best_w * _best_h << std::endl;
  output_file << _best_w << " " << _best_h << std::endl;
  output_file << 0 << std::endl; // TODO
  for (const auto& pair: _map_blk_to_id) {
    auto& [block_name, block_id] = pair;
    auto [center_x, center_y] = _best_centers[block_id];
    size_t block_w = _blocks_w_h[block_id][0], block_h = _blocks_w_h[block_id][1];
    size_t lb_x = (size_t(2 * center_x) - block_w) / 2, ru_x = lb_x + block_w;
    size_t lb_y = (size_t(2 * center_y) - block_h) / 2, ru_y = lb_y + block_h;
    output_file << pair.first << " " << lb_x << " " << lb_y << " " << ru_x << " " << ru_y << std::endl;
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

void FP::Simulator::_exchange_2_operands() {
  int tmp = PE[pick_op_0];
  PE[pick_op_0] = PE[pick_op_1];
  PE[pick_op_1] = tmp;
}

void FP::Simulator::_exchange_1_operator() {
  int ori_V_H = PE[pick_op_0];
  PE[pick_op_0] = (ori_V_H == -1) ? (-2) : (-1);      
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
  size_t tmp = PE[idx1];
  PE[idx1] = PE[idx2];
  PE[idx2] = tmp;
}

void FP::Simulator::_movement() {
  /**
   * two different movements
   * 1. exchange two operator (random)
   * 2. exchange operator from V (H) -> H (V)
  */
  // TODO: change this to follow the lecture slides
  // TODO: optimize this
  while (true) {
    _chosen_movement = random(0, 2);
    switch (_chosen_movement) {
      case 0: {
        // switch any two numbers
        do {
          pick_op_0 = random(0, PE.size() - 1);
        } while (PE[pick_op_0] < 0);
        do {
          pick_op_1 = random(0, PE.size() - 1);
        } while (PE[pick_op_1] < 0 || pick_op_0 == pick_op_1);

        // printf("_movement(): _num_blocks = %ld, pick_op_0 = %d, pick_op_1 = %d\n", _num_blocks, pick_op_0, pick_op_1);
        _PE_swap(pick_op_0, pick_op_1);
        break;
      }
      case 1: {
        // switch H <-> V
        do {
          pick_op_0 = random(0, PE.size() - 1); 
        } while (PE[pick_op_0] >= 0);

        // printf("_movement(): _num_operators = %ld, pick_op_0 = %d\n", _num_blocks-1, pick_op_0);
        _exchange_1_operator();
        break;
      }
      case 2: {
        // swap 2 adjacent operand and operator and check if it's valid
        do {
          pick_op_0 = random(0, PE.size() - 2);
          pick_op_1 = pick_op_0 + 1;
        } while ((PE[pick_op_0] >= 0 && PE[pick_op_1] >= 0) || PE[pick_op_0] == PE[pick_op_1]);
        _PE_swap(pick_op_0, pick_op_1);
        break;
      }
      default: {
        // programming error
        exit(1);
      }
    }

    // validate PE
    if (_chosen_movement == 2 && !_PE_validation()) {
      std::cout << "M3 is not valid" << std::endl;
      _PE_swap(pick_op_0, pick_op_0 + 1);
      continue;
    }
    break;
  }
  std::cout << "DEBUGGG: " << _chosen_movement << " " << pick_op_0 << " " << pick_op_1 << std::endl;
}

void FP::Simulator::_change_PE_back() {
  switch (_chosen_movement) {
    case 0: {
      _PE_swap(pick_op_0, pick_op_1);
      break;
    }
    case 1: {
      _exchange_1_operator();
      break;
    }
    case 2: {
      _PE_swap(pick_op_0, pick_op_1);
      break;
    }
    default: {
      // programming error
      exit(1);
      break;
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
  double T = 1.;
  double T_threshold = 0.001;
  double alpha = 0.85;
  size_t k = 5;
  // while (T > T_threshold) {
  for (size_t rnd = 0; rnd < 50; rnd++) { // debug
    std::cout << "start new SA round: T = " << T << std::endl;
    size_t bad_moves = 0; // TODO: maybe use eg -10 ?
    for (size_t i = 0; i < k * _num_blocks; i++) {
      std::cout << std::endl;
      _movement();
      _print_PE();

      double prev_cost = _cur_cost;
      _compute_cost();
      if (_cur_cost < prev_cost) {
        // better cost
        std::cout << "better cost, accept" << std::endl;
      } else {
        double prob = double(random(0, 99)) / 100;
        std::cout << "prob: " << prob << " " << T << std::endl;
        if (prob <= T) {
          // worse cost, but accept this change
          std::cout << "worse cost, accept" << std::endl;

        } else {
          std::cout << "worse cost, reject" << std::endl;
          bad_moves++;
          // if did not accpet the choice, need to change PE back
          _change_PE_back();
        }
      }
    }
    // decrease the temp
    // T *= alpha;

    // early break if too many bad moves
    // TODO: adjust 5 / 100
    std::cout << "tried = " << k * _num_blocks << ", bad_moves = " << bad_moves << "\n";
    // if (bad_moves >= double(k) * _num_blocks * 5 / 100) {
    //   break;
    // }
    // std::string xxx; std::cin >> xxx;
  }
}

void FP::Simulator::_update_best() {
  _best_length = _cur_length;
  _best_w = _cur_w;
  _best_h = _cur_h;
  _best_cost = _cur_cost;
  _best_centers = _cur_centers;
  _best_PE = PE;
}

void FP::Simulator::_compute_cost() {
  auto [total_w, total_h] = _compute_area(0, PE.size(), 0, 0);
  std::cout << "total: (w, h) = (" << total_w << ", " << total_h << ")\n";
  size_t area = total_w * total_h;
  
  double length = _compute_length();
  std::cout << "_compute_cost: (area, length) = (" << area << ", " << length << ")\n";

  // update norm if needed
  if (_norm_count < _norm_total) {
    // TODO: should we only include valid solutions?
    _area_norm = (_area_norm * _norm_count + area) / (_norm_count + 1);
    _length_norm = (_length_norm * _norm_count + length) / (_norm_count + 1);
    _norm_count++;
    std::cout << "norm updated: (area_norm, length_norm) = (" << _area_norm << ", " << _length_norm << ")\n";
  }

  _cur_length = length;
  _cur_w = total_w;
  _cur_h = total_h;

  // add penalty if out of bounds
  // TODO: adjust this
  double penalty_factor = 1;
  if (total_w > _constraint_width) {
    area += penalty_factor * (total_w - _constraint_width) * total_h;
  }
  if (total_h > _constraint_height) {
    area += penalty_factor * (total_h - _constraint_height) * total_w;
  }
  _cur_cost = _alpha * area / _area_norm + (1 - _alpha) * length / _length_norm;
  bool is_in_bound = total_w <= _constraint_width && total_h <= _constraint_height;

  if (_cur_cost < _best_cost && is_in_bound) {
    std::cout << "update best cost: " << _best_cost << std::endl;
    _update_best();
  }
}

std::pair<size_t, size_t> FP::Simulator::_compute_area(size_t start, size_t end, size_t base_x, size_t base_y) {
  if (end == start + 1) {
    size_t block_id = PE[start];
    size_t block_w = _blocks_w_h[block_id][0], block_h = _blocks_w_h[block_id][1];
    _cur_centers[block_id].first = base_x + double(block_w) / 2;
    _cur_centers[block_id].second = base_y + double(block_h) / 2;
    std::cout << "block " << PE[start] << " has a center point at (" <<  _cur_centers[block_id].first << ", " <<  _cur_centers[block_id].second << ")\n";
    return std::make_pair(block_w, block_h);
  }

  int count = 1, i;
  for (i = int(end) - 1; i >= int(start); i--) {
    count += (PE[i] >= 0) ? 1 : -1;
    if (count == 1) break;
  }
  size_t left_w, left_h;
  std::tie(left_w, left_h) = _compute_area(start, i, base_x, base_y);

  if (PE[end - 1] == -1) {
    // V case
    base_x += left_w;
  } else {
    // H case
    base_y += left_h;
  }

  size_t right_w, right_h;
  std::tie(right_w, right_h) = _compute_area(i, end - 1, base_x, base_y);

  if (PE[end - 1] == -1) {
    // V case
    return std::make_pair(left_w + right_w, std::max(left_h, right_h));
  } else {
    // H case
    return std::make_pair(std::max(left_w, right_w), left_h + right_h);
  }
}

double FP::Simulator::_compute_length() {
  // TODO: consider terminals
  double total_length = 0;
  for (const auto& blocks: _nets) {
    double min_x = _cur_centers[0].first, max_x = min_x;
    double min_y = _cur_centers[0].second, max_y = min_y;
    for (size_t block_id: blocks) {
      double cur_x = _cur_centers[block_id].first;
      double cur_y = _cur_centers[block_id].second;
      if (cur_x < min_x) {
        min_x = cur_x;
      } else if (cur_x > max_x) {
        max_x = cur_x;
      }
      if (cur_y < min_y) {
        min_y = cur_y;
      } else if (cur_y > max_y) {
        max_y = cur_y;
      }
    }
    std::cout << "(min_x, max_x) = (" << min_x << ", " << max_x << ")\n";
    std::cout << "(min_y, max_y) = (" << min_y << ", " << max_y << ")\n";
    total_length += (max_x - min_x) + (max_y - min_y);
  }
  return total_length;
}
