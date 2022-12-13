#include "Node.h"
#include "Position.h"
#include "Search.h"
#include "Util/Util.h"
#include "types.h"
#include <chrono>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string_view>

int main(int argl, const char **argc) {

  /* Interface inter;
  inter.loop();
 return 0;
*/
  /*
    Search<9> searchtest;
    searchtest.set_max_tree_size(1000000);
    searchtest.init();
    searchtest.set_max_nodes(10);
    searchtest.search();
    */

  // Thats not going to be easy to fix

  constexpr size_t board_size = 11;
  Search<board_size> search;
  search.set_max_tree_size(100000000);
  for (auto i = 0; i < board_size * board_size; ++i) {
    search.init();
    if (search.board.get_winner() != EMPTY)
      break;
    if (search.board.get_position().color == BLACK) {
      search.max_time = 100000;
      search.set_max_nodes(1000000000);

    } else {
      search.max_time = 100000;
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
              << ((float)~search.board.get_position().color) *
                     search.root->best_child()->get_win_over_visits()
              << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
