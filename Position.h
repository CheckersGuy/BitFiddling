//
// Created by robin on 03.11.21.
//

#ifndef READING_POSITION_H
#define READING_POSITION_H

#include <cstdint>
#include <cstdio>
#include  "BitPattern.h"
#include <immintrin.h>
#include <ostream>
#include "Prng.h"
#include <iostream>
#include "types.h"

template<size_t board_size>
struct Position {
    bit_pattern<board_size> BP;
    bit_pattern<board_size> WP;
    Color color{BLACK};
    size_t num_empty{board_size * board_size};


    static Position get_empty_position() {
        Position pos;
        return pos;
    }

    void make_move(int idx) {
        if (color == BLACK) {
            BP.set_bit(idx);
        } else {
            WP.set_bit(idx);
        }
        color = ~color;
        num_empty -= 1;
    }

    void unmake_move(int idx) {
        if (color == BLACK) {
            BP.clear_bit(idx);
        } else {
            WP.clear_bit(idx);
        }
        color = ~color;
        num_empty += 1;
    }


    size_t get_num_empty() {
        return num_empty;
    }

    inline Color get_square(int index) {
        if (BP.is_set(index))
            return BLACK;
        if (WP.is_set(index))
            return WHITE;

        return EMPTY;
    }

    inline uint64_t get_empty_squares(int index) {
        const uint64_t squares = BP.fields[index] | WP.fields[index];
        if (index == BP.fields.size() - 1) {
            return (~squares) & LEFT_MASK<board_size>;
        }
        return (~squares);
    }

    inline Color operator[](int index) {
        return get_square(index);
    }


    std::array<int, 6> get_neighbours(SquareType<board_size> hex_point) {
        //gets the neighbours of the particular hexpoint
        //using dummy values for edges
        std::array<int, 6> result;

    }

    inline size_t get_random_empty(Prng &generator) {
        //This function needs to be reworked
        uint64_t rand = generator();
        uint64_t index = rand % num_empty;

        uint64_t local_index = index;
        int field_index = 0;
        size_t count = 0;
        for (auto i = 0; i < BP.fields.size(); ++i) {
            size_t l_empt = get_empty_squares(i);
            size_t num = __builtin_popcountll(l_empt);
            count += num;
            if (count >= index + 1) {
                field_index = i;
                break;
            }
            local_index -= num;
        }

        uint64_t empty_squares = get_empty_squares(field_index);
        uint64_t index_mask = 1ull << local_index;
        uint64_t empty_square = _pdep_u64(index_mask, empty_squares);
        return _tzcnt_u64(empty_square) + 64ull * field_index;
    }

    void print() {
        for (auto i = 0; i < board_size; ++i) {
            for (auto p = 0; p < i; ++p) {
                std::cout << " ";
            }
            std::cout << "\\";
            std::cout << " ";
            for (auto k = 0; k < board_size; ++k) {
                if (BP.is_set(board_size * i + k)) {
                    std::cout << "B";
                } else if (WP.is_set(board_size * i + k)) {
                    std::cout << "W";
                } else {
                    std::cout << ".";
                }
                std::cout << " ";

            }
            std::cout << "\\";
            std::cout << "\n";
        }
    }

    Color get_mover() {
        return color;
    }

    friend std::ostream &operator<<(std::ostream &stream, const Position &pos) {
        stream << pos.BP;
        stream << pos.WP;
        stream.write((char *) &pos.color, sizeof(Color));
        return stream;
    }

    friend std::istream &operator>>(std::istream &stream, const Position &pos) {
        stream >> pos.BP;
        stream >> pos.WP;
        stream.read((char *) &pos.color, sizeof(Color));
        return stream;
    }


};


#endif //READING_POSITION_H
