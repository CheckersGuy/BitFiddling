//
// Created by robin on 05.11.21.
//

#ifndef READING_BOARD_H
#define READING_BOARD_H


#include "Position.h"
#include "Union.h"

Prng gen(12231ull);

template<size_t board_size>
class Board {

private:
    Union<board_size> un;
    Position<board_size> position;
    int last_move{-1};

public:


    void make_move(int index) {
        position.make_move(index);
        add_to_union(index);
    }

    void add_to_union(int index) {
        const Color index_color = position.get_square(index);
/*

        if (index == 10) {

            std::cout << position << std::endl;
            std::exit(-1);
        }
*/

        if (index_color == BLACK && is_north_edge<board_size>(index)) {
            un.merge(NORTH<board_size>, index);
        }
        if (index_color== BLACK && is_south_edge<board_size>(index)) {
            un.merge(SOUTH<board_size>, index);
        }
        if (index_color == WHITE && is_west_edge<board_size>(index)) {
            un.merge(WEST<board_size>, index);
        }
        if (index_color == WHITE && is_east_edge<board_size>(index)) {
            un.merge(EAST<board_size>, index);
        }

        if (!is_north_edge<board_size>(index) && get_position().get_square(index - 11) == index_color) {
            un.merge(index - 11, index);
        }
        if (!is_south_edge<board_size>(index) && get_position().get_square(index + 11) == index_color) {
            un.merge(index + 11, index);
        }
        if (!is_west_edge<board_size>(index) && get_position().get_square(index - 1) == index_color) {
            un.merge(index - 1, index);
            if (!is_south_edge<board_size>(index) && get_position().get_square(index + 10) == index_color) {
                un.merge(index + 10, index);
            }
        }

        if (!is_east_edge<board_size>(index) && get_position().get_square(index + 1) == index_color) {
            un.merge(index + 1, index);
            if (!is_north_edge<board_size>(index) && get_position().get_square(index - 10) == index_color) {
                un.merge(index - 10, index);
            }
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

    Color play_out() {
        const size_t empty_squares = get_position().get_num_empty();
        for (auto i = 0; i < empty_squares; ++i) {
            auto index = get_position().get_random_empty(gen);
            make_move(index);
        }
        return get_winner();
    }

    Position<board_size> &get_position() {
        return position;
    }

};


#endif //READING_BOARD_H
