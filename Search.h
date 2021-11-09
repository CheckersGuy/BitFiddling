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
    size_t max_time;
    size_t max_nodes;
    size_t max_tree_size;
    size_t nodes_in_tree{0};
    std::unique_ptr<Node<board_size>> root;
    Prng rand_source{111231231ull};
public:

    Search() {
        root = std::unique_ptr<Node<board_size>>(new Node<board_size>(board_size * board_size, nullptr));
    }

    void iterate() {
        Board<board_size> iter_board(board);
        Node<board_size> *current = root.get();

        while (current->get_num_children() != 0) {
            current = current->select(rand_source);
            iter_board.make_move(current->get_move());
        }

        if (iter_board.get_position().get_num_empty() != 0) {
            current->expand(iter_board.get_position());
            nodes_in_tree+=iter_board.get_position().get_num_empty();
            auto rand_index = rand_source() % current->get_num_children();
            current = (*current)[rand_index];
            iter_board.make_move(current->get_move());
        }
        Color mover = iter_board.get_position().get_mover();
        Color result = iter_board.play_out();
        current->back_up(result, mover);
    }

    void set_search_time(size_t time) {
        max_time = time;
    }

    void set_max_nodes(size_t nodes) {
        max_nodes = nodes;
    }

    size_t get_best_action(){
        Node<board_size> * best_child = root->best_child();
        return best_child->get_move();
    }

    void search() {
        if(board.get_winner()!=EMPTY)
            return;

        //resetting the root node before starting the search
        root = std::unique_ptr<Node<board_size>>(new Node<board_size>(board_size * board_size, nullptr));
        nodes_in_tree =0;
        size_t counter = 0;
        while (counter < max_nodes) {
            iterate();
            counter++;
        }
    }
};

#endif //READING_SEARCH_H
