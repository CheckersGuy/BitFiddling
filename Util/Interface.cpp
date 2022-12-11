//
// Created by robin on 18.02.22.
//

#include "Interface.h"

Interface::Interface() { search.set_max_tree_size(100000000); }

void Interface::loop() {
  // constantly reading from standard input
  // terminate once we receive a kill command

  std::string message;
  do {
    std::cin >> message;
    if (message == "clear_board") {
      clear_board();
    }
    if (message.starts_with("move")) {
      auto result = Util::split_by_delimiter(message, ":");
      std::string cmd = result[0];
      std::string value = result[1];
      int move_index = std::stoi(value);
      search.board.make_move(move_index);
      // search.board.get_position().print();
    }

    if (message.starts_with("search")) {
      auto result = Util::split_by_delimiter(message, ":");
      std::string cmd = result[0];
      std::string value = result[1];
      int search_time = std::stoi(value);
      search_position(search_time, 500000000);
      auto move = search.get_best_action();
      search.board.make_move(move);
      send_new_move(move);
      // search.board.get_position().print();
    }

  } while (message != "kill");
}

void Interface::clear_board() { search.board = Board<11>(); }

void Interface::receive_move(size_t move) { search.board.make_move(move); }

void Interface::send_new_move(size_t move) {
  std::cout << "Move:" << move << "\n";
}

void Interface::search_position(int max_time, int max_nodes) {
  search.init();
  search.set_max_nodes(max_nodes);
  search.set_max_time(max_time);
  search.init();
  search.search();
}
