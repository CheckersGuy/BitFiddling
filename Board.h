//
// Created by robin on 05.11.21.
//

#ifndef READING_BOARD_H
#define READING_BOARD_H


#include <vector>
#include "Position.h"
#include "Union.h"


template<size_t board_size>
class Board {

private:
    Union<board_size> un;
    Position<board_size> position;
    SquareType<board_size> last_move{board_size * board_size};

public:
    void make_move(int index) {
        position.make_move(index);
        add_to_union(index);
        last_move = index;
    }

    void add_to_union(int index) {
        const Color index_color = ~position.get_mover();

        if (index_color == BLACK && is_north_edge<board_size>(index)) {
            un.merge(NORTH<board_size>, index);
        }
        if (index_color == BLACK && is_south_edge<board_size>(index)) {
            un.merge(SOUTH<board_size>, index);
        }
        if (index_color == WHITE && is_west_edge<board_size>(index)) {
            un.merge(WEST<board_size>, index);
        }
        if (index_color == WHITE && is_east_edge<board_size>(index)) {
            un.merge(EAST<board_size>, index);
        }

        if (!is_north_edge<board_size>(index) && get_position().get_square(index - board_size) == index_color) {
            un.merge(index - board_size, index);
        }
        if (!is_south_edge<board_size>(index) && get_position().get_square(index + board_size) == index_color) {
            un.merge(index + board_size, index);
        }
        if (!is_west_edge<board_size>(index) && get_position().get_square(index - 1) == index_color) {
            un.merge(index - 1, index);
        }
        if (!is_west_edge<board_size>(index) && !is_south_edge<board_size>(index) &&
            get_position().get_square(index + board_size - 1) == index_color) {
            un.merge(index + board_size - 1, index);
        }

        if (!is_east_edge<board_size>(index) && get_position().get_square(index + 1) == index_color) {
            un.merge(index + 1, index);
        }
        if (!is_east_edge<board_size>(index) && !is_north_edge<board_size>(index) &&
            get_position().get_square(index - (board_size - 1)) == index_color) {
            un.merge(index - (board_size - 1), index);
        }
    }


    Color get_winner() {
        if (un.in_same_set(NORTH<board_size>, SOUTH<board_size>)) {
            return BLACK;
        } else if (un.in_same_set(WEST<board_size>, EAST<board_size>)) {
            return WHITE;
        } else {
            return EMPTY;
        }
    }

    Union<board_size> &get_union() {
        return un;
    }

    Color play_out(Prng &source) {
        Color winner = get_winner();
        while (winner == EMPTY) {
            auto index = get_position().get_random_empty(source);
            make_move(index);
            save_bridge(source, index);
            winner = get_winner();
        }
        return winner;
    }

    Position<board_size> &get_position() {
        return position;
    }

    void save_bridge(Prng &random_source, size_t hex_point) {
        if (is_on_edge<board_size>(hex_point)) {
            //will be done later
            return;
        }
            Position<board_size> neigh;


            size_t rand_start = random_source() % 6;
            Color opp_color = ~position.get_square(hex_point);
            auto neigh_bours = position.get_neighbours(hex_point);
            /*
                   for (auto sq: neigh_bours) {
                       neigh.BP.set_bit(sq);
                   }
           */
            size_t state = 0;
            size_t ret = 0;
            for (auto i = 0; i < 6; ++i) {
                auto index = rand_start + i;
                index = index % 6;
                auto value = neigh_bours[index];
                Color current;
                if (value == NORTH<board_size> || value == SOUTH<board_size>) {
                    current = BLACK;
                } else if (value == WEST<board_size> || value == EAST<board_size>) {
                    current = WHITE;
                } else {
                    current = position.get_square(value);
                }

                if (state == 0 && opp_color == current) {
                    state = 1;
                } else if (state == 1 && current == EMPTY) {
                    ret = neigh_bours[index];
                    state = 2;
                } else if (state == 2 && current == opp_color) {
                    /*       std::cout << "Index: " << hex_point << std::endl;
                           Position<board_size> test;
                           test.BP.set_bit(hex_point);
                           test.print();
                           neigh.print();
                           position.print();*/
                    make_move(ret);
                    /* position.print();
                     std::cout << std::endl;*/
                    break;
                } else {
                    state = 0;
                }
            }
        }


    };


#endif //READING_BOARD_H
