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


    void make_move(int idx) {
        if (color == BLACK) {
            BP.set_bit(idx);
        } else {
            WP.set_bit(idx);
        }
        color = ~color;
        num_empty -= 1;
    }


    size_t get_num_empty() {
        return num_empty;
    }

    Color get_square(int index) {
        if (BP.is_set(index))
            return BLACK;
        if (WP.is_set(index))
            return WHITE;

        return EMPTY;
    }

    uint64_t get_empty_squares(int index) {
        const uint64_t squares = BP.fields[index] | WP.fields[index];
        if (index == BP.fields.size() - 1) {
            return (~squares) & LEFT_MASK<board_size>;
        }
        return (~squares);
    }

    Color operator[](int index) {
        return get_square(index);
    }

    size_t get_random_empty(Prng &generator) {
        //This function needs to be reworked
        uint64_t rand = generator();
        /*  if(num_empty ==0){
              std::cout<<*this<<std::endl;
          }*/
        uint64_t index = rand % num_empty;

        uint64_t local_index = index;
        //Now we need to know where the empty square is
        int field_index = 0;
        size_t count = 0;
        //std::cout<<"RAnd: "<<index<<std::endl;
        for (auto i = 0; i < BP.fields.size(); ++i) {
            size_t l_empt = get_empty_squares(i);
            size_t num = __builtin_popcountll(l_empt);
            //std::cout<<"Empties: "<<num<<std::endl;
            count += num;
            //std::cout<<"Count: "<<count<<std::endl;
            if (count >= index + 1) {
                field_index = i;
                break;
            }
            local_index -= num;
        }
        //std::cout<<"Local: "<<local_index<<std::endl;
        //now wee need to select the empty square given by the index
        uint64_t empty_squares = get_empty_squares(field_index);
        uint64_t index_mask = 1ull << local_index;
        uint64_t empty_square = _pdep_u64(index_mask, empty_squares);
        //std::cout << "OUtput: " << _tzcnt_u64(empty_square) + 64ull * field_index << std::endl;
        return _tzcnt_u64(empty_square) + 64ull * field_index;
    }

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

    Color get_mover(){
        return color;
    }

};


#endif //READING_POSITION_H
