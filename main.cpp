#include <iostream>
#include <chrono>
#include <iterator>
#include "Position.h"
#include "types.h"
#include "Node.h"
#include "Search.h"
#include "NodeAllocator.h"

bool use_rave;

int main(int argl, const char **argc) {

    constexpr size_t board_size = 9;
    auto &allocator = NodeAllocator<board_size>::get_instance();
    allocator.init();
    using NType = Node<9>;

    NType* node =(NType *)allocator.allocate_children(121);
    node =(NType *)allocator.allocate_children(121);
    std::cout<<"Allocations:"<<allocator.num_allocations<<std::endl;
    std::cout<<"Offset: "<<allocator.offset<<std::endl;

    //allocator.release();




    return 0;


    //some computation


    /*  std::vector<std::unique_ptr<NType>> nodes;

      for (auto i = 0; i < 53000; i++) {
          nodes.emplace_back(std::make_unique<NType>());
      }
  */



    const size_t max_moves = board_size * board_size;

    Search<board_size> search;
    search.set_max_tree_size(100000000);
    for (auto i = 0; i < 1; ++i) {
        allocator.reset();
        search.init();
        if (search.board.get_winner() != EMPTY)
            break;

        if (search.board.get_position().color == BLACK) {
            use_rave = false;
            search.max_time = 1000;
            search.set_max_nodes(100000000);

        } else {
            use_rave = true;
            search.max_time = 1000;
            search.set_max_nodes(100000000);
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        search.search();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto count = (t2 - t1).count();
        std::cout << "Nodes:" << search.nodes_in_tree << std::endl;
        std::cout << "Time: " << count / 1000000 << std::endl;
        std::cout << "NumIterations: " << search.num_iterations << std::endl;
        auto action = search.get_best_action();
        search.board.make_move(action);
        search.board.get_position().print();
        std::cout << std::endl;
        std::cout << "Action: " << action << std::endl;
        std::cout << search.board.get_position().get_num_empty() << std::endl;
        std::cout << "Winner" << search.board.get_winner() << std::endl;
        auto pv = search.get_pv();
        std::copy(pv.begin(), pv.end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << std::endl;
        std::cout << "QValue: "
                  << ((float) ~search.board.get_position().color) * search.root->best_child()->get_win_over_visits()
                  << std::endl;
        std::cout << std::endl;


    }





    /*   Prng g(321231ull);

           for(auto k=0;k<10000000;++k){
               bit_pattern<11> pattern;
               std::vector<size_t> indices;
               for (auto i = 0; i < 30; ++i) {
                   auto rand = g() % 121;
                   if(std::find(indices.begin(),indices.end(),rand)==indices.end()){
                       indices.emplace_back(rand);
                       pattern.set_bit(rand);
                   }
               }
               std::sort(indices.begin(), indices.end());

               size_t count = 0;
               for (auto sq: pattern) {
                   if(indices[count]!=sq){
                       pattern.print();
                       std::exit(-1);
                   }

                   count++;
               }
           }
   */


    return 0;


    return 0;
}
