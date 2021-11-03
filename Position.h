//
// Created by robin on 03.11.21.
//

#ifndef READING_POSITION_H
#define READING_POSITION_H

#include <stdint.h>
#include <cstdio>
#include  "BitPattern.h"
#include <immintrin.h>
#include <ostream>
#include "Prng.h"
enum Color : int8_t {
    BLACK = -1, WHITE = 1
};

constexpr Color operator~(Color color) {
    return (color == BLACK) ? WHITE : BLACK;
}

template<size_t board_size>
struct Position {
    bit_pattern<board_size> BP;
    bit_pattern<board_size> WP;
    Color color{BLACK};
    size_t num_empty{board_size * board_size};


    void make_move(int idx);

    size_t get_num_empty();

    uint64_t get_empty_squares(int index);


    size_t get_random_empty(Prng &generator);

    friend std::ostream &operator<<(std::ostream &stream, Position &pos) {
        for (auto i = 0; i < board_size; ++i) {
            for (auto p = 0; p < i; ++p) {
                stream << " ";
            }
            stream << "\\";
            stream << " ";
            for (auto k = 0; k < board_size; ++k) {
                if (pos.BP.is_set(board_size * i + k)) {
                    stream << "B";
                } else if (pos.WP.is_set(board_size * i + k)) {
                    stream << "W";
                } else {
                    stream << ".";
                }
                stream << " ";

            }
            stream << "\\";
            stream << "\n";
        }
        return stream;
    }

};


#endif //READING_POSITION_H
