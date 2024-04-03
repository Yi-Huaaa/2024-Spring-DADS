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

  // for (size_t i = 0; i < _num_terminals; i++) {
  //   printf("terminal_%zu %zu %zu\n", i, _terminals_w_h[i][0], _terminals_w_h[i][1]);
  // } printf("\n");

  // // file 2
  // for (size_t i = 0; i < _num_nets; i++) {
  //   printf("Net_%zu, _net_degree = %zu:\n", i, _nets[i].size());
  //   for (size_t j = 0; j < _nets[i].size(); j++) {
  //     printf("%zu\n", _nets[i][j]);
  //   }
  // }
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
  std::iota(indices.begin()+start, indices.begin()+end, 0);

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
  std::cout << "area_sum = " << _area_sum << std::endl;
  std::cout << "bound = " << _constraint_width * _constraint_height << std::endl;
  std::cout << "ratio = " << double(_area_sum) / (_constraint_width * _constraint_height) << std::endl;

// // new new new new new new new new new new new new new new new new new

// // new new new new new new new new new new new new new new new new new
//   // Sort indices based on width (_blocks_w_h[0])
//   std::vector<size_t> sorted_indices_width = sort_indices(_blocks_w_h, 0);
  
//   std::cout << "Sorted indices based on width:" << std::endl;
//   for (size_t i : sorted_indices_width) {
//       std::cout << i+1 << " ";
//   } std::cout << std::endl;


//   // 以寬度為主
//   std::vector<std::vector<int>> same_blocks; 
//   same_blocks.push_back(std::vector<int>());
//   same_blocks[0].push_back(sorted_indices_width[0]);

//   for (size_t i = 1; i < sorted_indices_width.size(); i++) {
//     size_t add_block_idx = sorted_indices_width[i];
//     // printf("i = %ld, add_block_idx = %ld, same_blocks.size() = %ld\n", i, add_block_idx+1, same_blocks.size());
//     size_t now_w = _blocks_w_h[add_block_idx][0];
//     size_t now_h = _blocks_w_h[add_block_idx][1];
//     bool inside = false;
//     for (size_t j = 0 ; j < same_blocks.size(); j++) {
//       // printf("j = %ld\n", j);
//       size_t pre_block_idx = same_blocks[j][0];
//       size_t pre_w = _blocks_w_h[pre_block_idx][0];
//       size_t pre_h = _blocks_w_h[pre_block_idx][1];
//       // printf("now_w = %ld, now_h = %ld, pre_w = %ld, pre_h = %ld\n", now_w, now_h, pre_w, pre_h);
//       if ((now_w == pre_w) && (now_h == pre_h)) {
//         // printf("case0\n");
//         same_blocks[j].push_back(add_block_idx);
//         inside = true;
//         break;
//       }
//     }
//     if (!inside) {
//       // printf("case1\n");
//       same_blocks.push_back(std::vector<int>());
//       same_blocks[same_blocks.size()-1].push_back(add_block_idx);    
//     }
//     // printf("\n\n");
//   }

//   printf("same_blocks:\n");
//   for (size_t i = 0; i < same_blocks.size(); i++) {
//     printf("%ld: ", i);
//     for (size_t j = 0; j < same_blocks[i].size(); j++) {
//       printf("%d, ", same_blocks[i][j]+1);
//     }
//     printf("\n");
//   }

//   // 根據寬度大小，一直往高度塞
//   std::vector<std::vector<size_t>> remain_regions; // w, h
//   size_t height_accum = 0;
//   size_t h_acc_num = 0;


//   // 這裡的 update 也要多個 case
//   while ((height_accum < _constraint_height) && (h_acc_num < same_blocks.size())) {
//     printf("same_blocks[h_acc_num][0] = %ld\n", same_blocks[h_acc_num][0]+1);
//     size_t h = _blocks_w_h[same_blocks[h_acc_num][0]][1];
//     height_accum += (h)*same_blocks[h_acc_num].size();
//     printf("(h)*same_blocks[h_acc_num].size() = %ld, h = %ld, same_blocks[h_acc_num].size() = %ld\n", (h)*same_blocks[h_acc_num].size(), h, same_blocks[h_acc_num].size());
//     remain_regions.push_back(std::vector<size_t>());
//     printf("_constraint_width = %ld, add = %ld\n", _constraint_width, _blocks_w_h[same_blocks[h_acc_num][0]][0]);
//     remain_regions[h_acc_num].push_back(_constraint_width - _blocks_w_h[same_blocks[h_acc_num][0]][0]); // w
//     remain_regions[h_acc_num].push_back(_constraint_height - (h)*same_blocks[h_acc_num].size()); // h
//     h_acc_num++;
//     printf("?!height_accum = %ld, _constraint_height = %ld\n", height_accum, _constraint_height);
//   }
  
//   if (height_accum > _constraint_height) {
//     h_acc_num--;
//     size_t h = _blocks_w_h[same_blocks[h_acc_num][0]][1];
//     height_accum -= (h)*same_blocks[h_acc_num].size();
//     remain_regions.pop_back();
//   }

//   printf("height_accum = %ld, h_acc_num = %ld, _constraint_height = %ld\n", height_accum, h_acc_num, _constraint_height);

//   printf("wid_accum:\n");
//   for (size_t i = 0; i < remain_regions.size(); i++) {
//     printf("i = %ld, remain_w = %ld, remain_h = %ld\n", i, remain_regions[i][0], remain_regions[i][1]);
//   }
  

//   std::vector<std::vector<size_t>> final_packing_results; final_packing_results.resize(h_acc_num);
//   for (size_t i = 0; i < h_acc_num; i++) {
//     for (size_t k = 0; k < same_blocks[i].size(); k++) {
//       final_packing_results[i].push_back(same_blocks[i][k]);
//     }
//   }
//   printf("init final_packing_results:\n");
//   for (size_t i = 0; i < final_packing_results.size(); i++) {
//     printf("%ld: ", i);
//     for (size_t j = 0; j < final_packing_results[i].size(); j++) {
//       printf("%ld, ", final_packing_results[i][j]+1);
//     }
//     printf("\n");
//   }  


//   std::vector<bool> whether_rotate; whether_rotate.resize((same_blocks.size() - h_acc_num + 1), false);
//   for (size_t i = h_acc_num; i < same_blocks.size(); i++) {
//     // 開始嘗試一邊 rotate 一邊塞進去
//     // case 0: 疊橫 不轉
//     size_t added_w_0 = (_blocks_w_h[same_blocks[i][0]][0])*same_blocks[i].size();
//     size_t added_h_0 = (_blocks_w_h[same_blocks[i][0]][1]);

//     // case 1: 疊直 不轉
//     size_t added_w_1 = (_blocks_w_h[same_blocks[i][0]][0]);
//     size_t added_h_1 = (_blocks_w_h[same_blocks[i][0]][1])*same_blocks[i].size();

//     // case 2: 疊橫 轉
//     size_t added_w_2 = added_h_0;
//     size_t added_h_2 = added_w_0;

//     // case 3: 疊直 轉
//     size_t added_w_3 = added_h_1;
//     size_t added_h_3 = added_w_1;
    
//     // printf("! i = %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld\n", i, added_w_0, added_h_0, added_w_1, added_h_1, added_w_2, added_h_2, added_w_3, added_h_3);
//     for (size_t j = 0; j < remain_regions.size(); j++) { // 已經存在的大型 blocks 剩下的區域看看塞不塞得進去
//       size_t remain_w = remain_regions[j][0];
//       size_t remain_h = remain_regions[j][1];
//       // printf("j = %ld, remain_w = %ld, remain_h = %ld\n", j, remain_w, remain_h);
//       // case 0
//       if ((added_w_0 <= remain_w) && (added_h_0 <= remain_h)) { // case 0
//         for (size_t k = 0; k < same_blocks[i].size(); k++) {
//           final_packing_results[j].push_back(same_blocks[i][k]);
//         }
//         remain_regions[j][0] -= added_w_0;
//         remain_regions[j][1] -= added_h_0;
//         whether_rotate[j] = false;
//         // printf("Merge! case 0\n");
//         break;
//       } else if ((added_w_1 <= remain_w) && (added_h_1 <= remain_h)) { 
//         for (size_t k = 0; k < same_blocks[i].size(); k++) {
//           final_packing_results[j].push_back(same_blocks[i][k]);
//         }
//         remain_regions[j][0] -= added_w_1;
//         remain_regions[j][1] -= added_h_1;
//         whether_rotate[j] = false;
//         // printf("Merge! case 1\n");
//         break;
//       } else if ((added_w_2 <= remain_w) && (added_h_2 <= remain_h)) { // case 2
//         for (size_t k = 0; k < same_blocks[i].size(); k++) {
//           final_packing_results[j].push_back(same_blocks[i][k]);
//         }
//         remain_regions[j][0] -= added_w_2;
//         remain_regions[j][1] -= added_h_2;
//         whether_rotate[j] = true;
//         // printf("Merge! case 2\n");
//         break;
//       } else if ((added_w_3 <= remain_w) && (added_h_3 <= remain_h)) { // case 3
//         for (size_t k = 0; k < same_blocks[i].size(); k++) {
//           final_packing_results[j].push_back(same_blocks[i][k]);
//         }
//         remain_regions[j][0] -= added_w_3;
//         remain_regions[j][1] -= added_h_3;
//         whether_rotate[j] = true;
//         // printf("Merge! case 3\n");
//         break;
//       }
//       // printf("\n");
//     }
//     // printf("\n\n");
//   }

//   printf("FINAL final_packing_results:\n");
//   for (size_t i = 0; i < final_packing_results.size(); i++) {
//     printf("%ld: ", i);
//     for (size_t j = 0; j < final_packing_results[i].size(); j++) {
//       printf("%ld, ", final_packing_results[i][j]+1);
//     }
//     printf("\n");
//   }  

//   // whether_rotate
//   for (size_t i = 0; i < whether_rotate.size(); i++) {
//     if (whether_rotate[i])
//       printf("whether_rotate[%ld] = true\n", i);
//     else
//       printf("whether_rotate[%ld] = false\n", i);
//   }

//   // test 能不能找到所有人
//   std::vector<bool> find_all_blocks; find_all_blocks.resize(_num_blocks, false);
//   for (size_t i = 0; i < final_packing_results.size(); i++) {
//     for (size_t j = 0; j < final_packing_results[i].size(); j++) {
//       find_all_blocks[final_packing_results[i][j]] = true;
//     }
//   }

//   for (size_t i = 0; i < find_all_blocks.size(); i++) {
//     if (find_all_blocks[i] == false) {
//       printf("ERROR: %ld miss in the rectrangular graph\n", i+1);
//       exit(1);
//     }
//   }

//   PE.clear();
//   PE = {6, 7, -2, 1, -1, 9, 11, -2, 2, -1, 8, 10, -2, 3, 4, -2, 5, -2, -1, -2, -2};
//   for (size_t i = 0; i < PE.size(); i++) {
//     if (PE[i]>0)
//         PE[i]--;
//   }

  


//   // whether_rotate
//   size_t need_to_roate[2] = {0, 1}; // block_idx
//   for (size_t i = 0; i < 2; i++) {
//     size_t idx = need_to_roate[i];
//     size_t tmp = _blocks_w_h[idx][0];
//     _blocks_w_h[idx][0] = _blocks_w_h[idx][1];
//     _blocks_w_h[idx][1] = tmp;
//   }  

//   printf(":bibi\n");
//   _print_PE();
//   bool a = _PE_validation();
//   if (a == false) {
//     printf("Q\n");
//     exit(1);
//   }

 
//   // init norm variables
//   _norm_count = 0;
//   _norm_total = 10; // TODO: adjust this based on prob size
//   _area_norm = 0;
//   _length_norm = 0;

//   // compute the cost of the initial PE
//   _cur_centers.resize(_num_blocks);
//   _best_lb.resize(_num_blocks);
//   _best_ru.resize(_num_blocks);
//   _compute_cost();
//   _update_best(); 

  // for (size_t i = 0; i < find_all_blocks.size(); i++) {
  //   if (find_all_blocks[i] == false) {
  //     printf("ERROR: %ld miss in the rectrangular graph\n", i+1);
  //     exit(1);
  //   }
  // }
  

//   // use width as major 
//   size_t hei_accum = 0, add_num_counter = 0;
//   std::vector<std::vector<int>> block_pairs;
//   std::vector<bool> picked_blocks; picked_blocks.resize(_num_blocks, false);
//   while (hei_accum < _constraint_height) {
//     block_pairs.push_back(std::vector<int>());

//     hei_accum += _blocks_w_h[sorted_indices_height[add_num_counter]][1];
//     block_pairs[add_num_counter].push_back(sorted_indices_height[add_num_counter]);
//     picked_blocks[sorted_indices_height[add_num_counter]] = true;
//     add_num_counter++;
//   } add_num_counter--;
  
//   printf("hei_accum = %ld, add_num_counter = %ld\n", hei_accum, add_num_counter);
//   for (size_t i = 0 ; i < add_num_counter; i++) {
//     printf("%ld, ", sorted_indices_height[i]);
//   } printf("\n");

//   printf("pre block_pairs:\n");
//   for (size_t i = 0; i < block_pairs.size(); i++) {
//     for (size_t j = 0; j < block_pairs[i].size(); j++) {
//       printf("%d, ", block_pairs[i][j]+1);
//     } printf("\n");
//   } printf("\n");

//   std::queue<int> picked_block_candidates; 
//   for (size_t i = 0; i < sorted_indices_width.size(); i++) {
//     if (!picked_blocks[sorted_indices_width[i]]) {
//       picked_block_candidates.push(sorted_indices_width[i]);
//     }
//   }
//   // greedy solution to pack the block pairs 
//   printf("block_pairs.size() = %ld\n", block_pairs.size());
//   for (size_t i = 0; i < block_pairs.size(); i++) {
//     size_t wei_accum = _blocks_w_h[block_pairs[i][0]][0];
//     if ((wei_accum < _constraint_width)) {
//       size_t tried = 0;
//       // printf("wei_accum = %ld, _constraint_width = %ld\n", wei_accum, _constraint_width);
//       while (tried < picked_block_candidates.size()) {
//         int candidate_block = picked_block_candidates.front();
//         picked_block_candidates.pop();
//         tried++;
//         // printf("tried = %ld, wei_accum = %ld, _blocks_w_h[%d][0] = %ld\n\n", tried, wei_accum, candidate_block, _blocks_w_h[candidate_block][0]);
//         if ((wei_accum + _blocks_w_h[candidate_block][0]) <= _constraint_width) {
//           block_pairs[i].push_back(candidate_block);
//           wei_accum += _blocks_w_h[candidate_block][0];
//         } else {
//           picked_block_candidates.push(candidate_block);
//         }
//       }
//     }
//   }

//   printf("post: block_pairs:\n");
//   for (size_t i = 0; i < block_pairs.size(); i++) {
//     for (size_t j = 0; j < block_pairs[i].size(); j++) {
//       printf("%d, ", block_pairs[i][j]+1);
//     } printf("\n");
//   } printf("\n");

//   std::vector<bool> test_all_get; test_all_get.resize(_num_blocks, false);
//   for (size_t i = 0; i < block_pairs.size(); i++) {
//     for (size_t j = 0; j < block_pairs[i].size(); j++) {
//       test_all_get[block_pairs[i][j]] = true;
//     } 
//   } 

// #ifdef FP_INIT_CEHCK
//   printf("bi PE:\n");
//   _print_PE();
// #endif
//   bool aa = _PE_validation();
//   printf("aa = %B\n", aa);

//   // init norm variables
//   _norm_count = 0;
//   _norm_total = 100; // TODO: adjust this based on prob size
//   _area_norm = 0;
//   _length_norm = 0;

//   // compute the cost of the initial PE
//   _cur_centers.resize(_num_blocks);
//   _compute_cost();
//   _update_best();


//   // new new new new new new new new new new new new new new new new new

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
  /**
   * two different movements
   * 1. exchange two operator (random)
   * 2. exchange operator from V (H) -> H (V)
  */
  // TODO: change this to follow the lecture slides
  // TODO: optimize this
  // while (true) {
  //   _chosen_movement = random(0, 2);
  //   switch (_chosen_movement) {
  //     case 0: {
  //       // switch any two numbers
  //       // do {
  //       //   pick_op_0 = random(0, PE.size() - 1);
  //       // } while (PE[pick_op_0] < 0);
  //       // do {
  //       //   pick_op_1 = random(0, PE.size() - 1);
  //       // } while (PE[pick_op_1] < 0 || pick_op_0 == pick_op_1);
  //       // _PE_swap(pick_op_0, pick_op_1);

  //       // swap adjacent numbers
  //       do {
  //         pick_op_0 = random(0, PE.size() - 2);
  //         pick_op_1 = pick_op_0 + 1;
  //       } while (PE[pick_op_0] < 0 || PE[pick_op_1] < 0);
  //       // pick_op_0 and pick_op_1 should be numbers
  //       _PE_swap(pick_op_0, pick_op_1);
  //       break;
  //     }
  //     case 1: {
  //       // switch H <-> V
  //       // do {
  //       //   pick_op_0 = random(0, PE.size() - 1); 
  //       // } while (PE[pick_op_0] >= 0);
  //       // _exchange_1_operator();
  //       do {
  //         pick_op_0 = random(1, PE.size() - 1);
  //       } while (PE[pick_op_0] >= 0 || PE[pick_op_0 - 1] < 0);
  //       // pick_op_0 is H/V and pick_op_0 - 1 is a number
  //       _PE_complement_chain(pick_op_0);
  //       break;
  //     }
  //     case 2: {
  //       // swap 2 adjacent operand and operator and check if it's valid
  //       do {
  //         pick_op_0 = random(0, PE.size() - 2);
  //         pick_op_1 = pick_op_0 + 1;
  //       } while ((PE[pick_op_0] >= 0 && PE[pick_op_1] >= 0) || PE[pick_op_0] == PE[pick_op_1]);
  //       _PE_swap(pick_op_0, pick_op_1);
  //       break;
  //     }
  //     default: {
  //       // programming error
  //       exit(1);
  //     }
  //   }

  //   // validate PE
  //   if (_chosen_movement == 2 && !_PE_validation()) {
  //     // std::cout << "M3 is not valid" << std::endl;
  //     _PE_swap(pick_op_0, pick_op_1);
  //     continue;
  //   }
  //   break;
  // }
  // // std::cout << "DEBUGGG: " << _chosen_movement << " " << pick_op_0 << " " << pick_op_1 << std::endl;

  // bi
  bool valid_movement = false;
  while (!valid_movement) {
    _chosen_movement = random(0, 3);
    if (_chosen_movement == 0) { // movement_0: swap adjacent numbers
      size_t failed_find_times = 0;
      while (failed_find_times < 10) {
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
      while (true) {
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
  size_t tried_count = 0;
  double T = 1000;
  double T_threshold = 0.01; // 決定要跑幾 round
  double alpha = 0.95;
  size_t k = 1000;
  size_t rnd = 0;
  while (T > T_threshold) {
  // for (size_t rnd = 0; rnd < 5000; rnd++) { // debug
    // std::cout << "start new SA round: T = " << T << std::endl;
    if (rnd % 1000 == 0) std::cout << "rnd = " << rnd << std::endl;
    rnd++;
    size_t bad_moves = 0; // TODO: maybe use eg -10 ?
    for (size_t i = 0; i < k * _num_blocks; i++) {
      tried_count += 1;
      // std::cout << std::endl;
      // _print_PE();
      // for (size_t i = 0; i < _blocks_w_h.size(); i++) {
      //   std::cout << i << " " << _blocks_w_h[i][0] << " " << _blocks_w_h[i][1] << "\n";
      // }
      _movement();
      // _print_PE();
      // for (size_t i = 0; i < _blocks_w_h.size(); i++) {
      //   std::cout << i << " " << _blocks_w_h[i][0] << " " << _blocks_w_h[i][1] << "\n";
      // }

      double prev_cost = _cur_cost;
      _compute_cost();
      if (_cur_cost < prev_cost) {
        // better cost
        // std::cout << "better cost, accept" << std::endl;
      } else {
        double prob = double(rand()) / RAND_MAX;
        // std::cout << "prob: " << prob << " " << T << std::endl;
        // TODO: try <= exp(-∆L/T)
        if (prob <= T) {
          // worse cost, but accept this change
          // std::cout << "worse cost, accept" << std::endl;

        } else {
          // std::cout << "worse cost, reject" << std::endl;
          bad_moves++;
          // if did not accpet the choice, need to change PE back
          _change_PE_back();
        }
      }
    }
    // decrease the temp
    T *= alpha;

    // early break if too many bad moves
    // TODO: adjust 5 / 100
    // std::cout << "tried = " << k * _num_blocks << ", bad_moves = " << bad_moves << "\n";
    // if (bad_moves >= double(k) * _num_blocks * 5 / 100) break;
  }
  std::cout << "tried_count = " << tried_count << std::endl;
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
    printf("i = %ld, lb_x = %ld, lb_y = %ld\n", i, lb_x, lb_y);
    std::cout << "center_x " << center_x << ", center_y " << center_y << "\n";
    std::cout << "block_w " << block_w << ", block_h " << block_h << "\n";
  }
  _best_PE = PE;
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
  // TODO: dynamic penalty factor
  double penalty_factor = 10000; // TODO: adjust this
  if (total_w > _constraint_width) {
    // area += penalty_factor * (total_w - _constraint_width) * total_h;
    length += penalty_factor * (total_w - _constraint_width) * total_h;
  }
  if (total_h > _constraint_height) {
    // area += penalty_factor * (total_h - _constraint_height) * total_w;
    length += penalty_factor * (total_h - _constraint_height) * total_w;
  }
  _cur_cost = _alpha * area / _area_norm + (1 - _alpha) * length / _length_norm;
  bool is_in_bound = total_w <= _constraint_width && total_h <= _constraint_height;

  // check if can update best
  if (_cur_cost < _best_cost && is_in_bound) {
    std::cout << "FOUND IN BOUND!!!\n";
    std::cout << "update best cost: " << _best_cost << std::endl;
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

    // HPWL
    total_length += (max_x - min_x) + (max_y - min_y);
  }
  return total_length;
}
