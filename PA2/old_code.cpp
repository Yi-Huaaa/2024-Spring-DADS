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