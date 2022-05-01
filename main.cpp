#include <iostream>
#include <chrono>
#include <iterator>
#include "Position.h"
#include "types.h"
#include "Node.h"
#include "Search.h"
#include "NodeAllocator.h"
#include <string_view>
#include <ranges>
#include "Util/Interface.h"
#include "Util/Util.h"

int main(int argl, const char **argc) {

    Interface inter;
    inter.loop();
    return 0;



//40-45k simulations per second with backup function
//next testing with backup2



    constexpr size_t board_size = 9;
    auto &allocator = NodeAllocator<board_size>::get_instance();


    Search<board_size> search;
    search.set_max_tree_size(100000000);
    for (auto i = 0; i < board_size*board_size; ++i) {
        allocator.reset();
        search.init();
        if (search.board.get_winner() != EMPTY)
            break;
        if (search.board.get_position().color == BLACK) {
            search.max_time = 1000;
            search.set_max_nodes(1000000000);

        } else {
            search.max_time = 10000;
            search.set_max_nodes(1000000000);
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


    return 0;

}
