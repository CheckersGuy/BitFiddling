//
// Created by robin on 09.11.21.
//

#ifndef READING_SEARCH_H
#define READING_SEARCH_H

#include "Board.h"
#include "Prng.h"

template<size_t board_size>
class Search {

public:
    Board<board_size> board;
    size_t max_time{10000000ull};
    size_t max_nodes;
    size_t max_tree_size;
    size_t nodes_in_tree{0};
    std::unique_ptr<Node<board_size>> root;
    Prng rand_source;
public:

    Search() : rand_source(std::chrono::duration_cast<std::chrono::milliseconds>
                                   (std::chrono::high_resolution_clock::now().time_since_epoch()).count()) {
        root = std::unique_ptr<Node<board_size>>(new Node<board_size>(board_size * board_size, nullptr));
    }

    void iterate() {
        Board<board_size> iter_board(board);
        Node<board_size> *current = root.get();

        while (current->get_num_children() != 0 && !current->is_terminal()) {
            current = current->select(rand_source);
            iter_board.make_move(current->get_move());
        }
        bit_pattern<board_size> after_WP;
        bit_pattern<board_size> after_BP;


        Color mover = iter_board.get_position().get_mover();
        Color winner = iter_board.get_winner();

/*
        if (current->is_terminal()) {
            current->back_up((current->is_won()) ? mover : ~mover, mover, after_WP, after_BP);
            return;
        }
*/
        if (!current->is_terminal() && winner != 0) {
            current->make_terminal((mover == winner) ? WIN : LOSS);
        }



        if (!current->is_terminal() && iter_board.get_position().get_num_empty() != 0 &&
            nodes_in_tree < max_tree_size) {
            current->expand(iter_board.get_position());
            nodes_in_tree += iter_board.get_position().get_num_empty();
            auto rand_index = rand_source() % current->get_num_children();
            current = (*current)[rand_index];
            iter_board.make_move(current->get_move());
        }
        mover = iter_board.get_position().get_mover();
        //getting empty squares before the playout for the rave_update

        bit_pattern<board_size> before_WP = iter_board.get_position().WP;
        bit_pattern<board_size> before_BP = iter_board.get_position().BP;
        Color result = iter_board.play_out(rand_source);
        after_WP = iter_board.get_position().WP;
        after_BP = iter_board.get_position().BP;

        after_WP = after_WP & (~before_WP);
        after_BP = after_BP & (~before_BP);
        current->back_up(result, mover, after_WP, after_BP);
    }

    void set_search_time(size_t time) {
        max_time = time;
    }

    std::vector<SquareType<board_size>> get_pv() {
        std::vector<SquareType<board_size>> result;
        Node<board_size> *current = root.get();
        while (current != nullptr && current->get_num_children() != 0) {
            current = current->best_child();
            result.emplace_back(current->move);
        }
        return result;
    }

    void set_max_tree_size(size_t nodes) {
        max_tree_size = nodes;
    }

    void set_max_nodes(size_t nodes) {
        max_nodes = nodes;
    }

    size_t get_best_action() {
        Node<board_size> *best_child = root->best_child();
        return best_child->get_move();
    }

    void search() {
        if (board.get_winner() != EMPTY)
            return;

        //resetting the root node before starting the search
        auto t1 = std::chrono::high_resolution_clock::now();
        root = std::unique_ptr<Node<board_size>>(new Node<board_size>(board_size * board_size, nullptr));
        auto t2 = std::chrono::high_resolution_clock::now();
        std::cout << "Deallocation " << (t2 - t1).count() / 1000000 << std::endl;
        nodes_in_tree = 0;
        size_t counter = 0;
        auto start = std::chrono::high_resolution_clock::now();
        while (counter < max_nodes) {
            auto end = std::chrono::high_resolution_clock::now();
            auto time_used = (end - start).count() / 1000000;
            if (time_used >= max_time)
                break;
            iterate();
            counter++;
        }
    }
};

#endif //READING_SEARCH_H
