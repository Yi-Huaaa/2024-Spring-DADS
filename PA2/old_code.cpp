// void FP::Simulator::_compuate_area() {
//   _area = 1;
  /**
   * traverse PE
   * 先分出左右子樹
   * 接著一路做 左子樹
   * 
   * */ 

  // vector: pop_back
// -----------  // 
//   std::vector<std::vector<int>> left_tree;
//   std::vector<int> left_tree_operators;
//   size_t accum = 0;
//   size_t left_tree_idx = 0;
  
//   while (accum < PE.size()) {
//     // printf("pre: left_tree.size() = %ld\n", left_tree.size());
//     if (PE[accum] >= 0) 
//       left_tree.push_back(std::vector<int>()); // push an empty vector
//     while (PE[accum] >= 0) {
//       left_tree[left_tree_idx].push_back(PE[accum]);
//       accum++;
//     }
//     left_tree_idx++;
//     left_tree_operators.push_back(PE[accum]);
//     accum++;
//     // printf("post: left_tree.size() = %ld\n", left_tree.size());
//   }

//   printf("left_tree:\n");
//   for (size_t i = 0; i < left_tree.size(); i++) {
//     printf("left_tree[%ld]: ", i);
//     for (size_t j = 0; j < left_tree[i].size(); j++) {
//       printf("%d, ", left_tree[i][j]);
//     } printf("\n");
//   }
//   printf("left_tree_operators: ");
//   for (int x: left_tree_operators) {
//     if (x == -1) std::cout << "V ";
//     else if (x == -2) std::cout << "H ";
//     else std::cout << x << " ";
//   }; printf("\n");

// #ifdef FP_CHECK_compuate_area
//   std::vector<int> test_PE = {0, 1, -1, 2, 3, -2, 4, 5, -2, 6, -1, -1, -2,};
//   for (int x: test_PE) {
//     if (x == -1) std::cout << "V ";
//     else if (x == -2) std::cout << "H ";
//     else std::cout << x << " ";
//   };
//   std::cout << std::endl;

//   printf("left_tree:\n");
//   for (size_t i = 0; i < left_tree.size(); i++) {
//     printf("left_tree[%ld]: ", i);
//     for (size_t j = 0; j < left_tree[i].size(); j++) {
//       printf("%d, ", left_tree[i][j]);
//     } printf("\n");
//   }
//   printf("left_tree_operators: ");
//   for (int x: left_tree_operators) {
//     if (x == -1) std::cout << "V ";
//     else if (x == -2) std::cout << "H ";
//     else std::cout << x << " ";
//   }; printf("\n");
// #endif

//   // vector: pop_back
//   // 接著一路做 左子樹

//   // for (size_t i = 0; i < left_tree.size(); i++) {
//   //   int l_idx = left_tree[i][0]; // left children index
//   //   int r_idx = left_tree[i][1]; // right children index

//   // }
// }

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

// printf("record_base_x and record_base_x\n");
  // for (size_t i = 0; i < record_base_x.size(); i++) {
  //   printf("block_%ld, base_x = %ld, base_y = %ld\n", i, record_base_x[i], record_base_y[i]);
  // }

  // std::vector<size_t> implemented_order; implemented_order.resize(_num_blocks);
  // // 這裡要補東西：implemented_order


  // // constrcut PE
  // std::list<int> _PE_tmp;
  // std::vector<size_t> start_points; 
  // std::vector<bool> _selected_as_start_pt; _selected_as_start_pt.resize(_num_blocks, false);
  // int vertical = -1, horizon = -2;

  // // the first implemented block
  // _PE_tmp.push_back(implemented_order[0]);
  // start_points.push_back(implemented_order[0]);
  // // remaining implemented blocks
  // for (size_t i = 1; i < implemented_order.size(); i++) {
  //   size_t block_idx = implemented_order[i];
  //   // find the start point
  //   // V
  //   for (size_t j = 0; j < start_points.size(); j++) {
  //     size_t x_now = record_base_x[block_idx];
  //     size_t y_now = record_base_y[block_idx];
  //     size_t x_minus_w = x_now - _blocks_w_h[start_points[j]][0];
  //     if ((x_minus_w == record_base_x[j]) && (y_now == record_base_y[start_points[j]])) {
  //       if (!_selected_as_start_pt[start_points[j]]) { // 沒有被選過，直接 insert
  //         _PE_tmp.push_back(block_idx); // 錯了，這裡不能直接插入，要找到相對應的地方插入
  //         _PE_tmp.push_back(vertical);          
  //       } else { // 不然就要往後 traverse 一路到連續兩個 負數 的地方往後插入

  //       }

  //     }
  //   }
  // }
