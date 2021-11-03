//
// Created by robin on 03.11.21.
//

#include "Position.h"


template<size_t board_size>
void Position<board_size>::make_move(int idx) {
    if (color == BLACK) {
        BP.set_bit(idx);
    } else {
        WP.set_bit(idx);
    }
    color = ~color;
    num_empty -= 1;
}

template<size_t board_size>
size_t Position<board_size>::get_num_empty() {
    return num_empty;
}

template<size_t board_size>
uint64_t Position<board_size>::get_empty_squares(int index) {
    const uint64_t squares = BP.fields[index] | WP.fields[index];
    if (index == BP.fields.size() - 1) {
        return (~squares) & LEFT_MASK<board_size>;
    }
    return (~squares);
}

template<size_t board_size>
size_t Position<board_size>::get_random_empty(Prng &generator) {
    uint64_t rand = generator();
    uint64_t index = rand % num_empty;
    //std::cout << "Index: " << index << std::endl;
    //Now we need to know where the empty square is
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
        index = index - num;
    }
    //now wee need to select the empty square given by the index
    uint64_t empty_squares = get_empty_squares(field_index);
    uint64_t index_mask = 1ull << index;
    uint64_t empty_square = _pdep_u64(index_mask, empty_squares);
    //std::cout << "OUtput: " << _tzcnt_u64(empty_square) + 64ull * field_index << std::endl;
    return _tzcnt_u64(empty_square) + 64ull * field_index;
}
