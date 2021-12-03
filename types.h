//
// Created by robin on 07.11.21.
//

#ifndef READING_TYPES_H
#define READING_TYPES_H

#include <cstdint>
//here go some definitions like constants and helper functions
// and definition of certain types


//Squaretype is for example used in the Union class. However, when the board-size increases, 8 bit are
//not enough to represent board_size**2 values.
bool use_rave = false;
template<size_t board_size> using SquareType =std::conditional_t<board_size <= 15, uint8_t , uint16_t>;


template<size_t board_size>
bool is_north_edge(size_t idx) {
    size_t row = (idx / board_size);
    return row == 0;
}

template<size_t board_size>
bool is_south_edge(size_t idx) {
    size_t row = (idx / board_size);
    return row == (board_size - 1);
}

template<size_t board_size>
bool is_west_edge(size_t idx) {
    size_t col = (idx % board_size);
    return col == 0;
}

template<size_t board_size>
bool is_east_edge(size_t idx) {
    size_t col = (idx % board_size);
    return col == (board_size - 1);
}


template<size_t board_size>
constexpr uint64_t get_board_mask() {
    size_t num_squares = board_size * board_size;
    size_t left_over = num_squares % 64ull;
    left_over = ((left_over == 0) ? 64 : left_over);
    uint64_t maske = 0ull;
    for (auto i = 0; i < left_over; ++i) {
        maske |= 1ull << i;
    }
    return maske;
}

template<size_t board_size> constexpr uint64_t LEFT_MASK = get_board_mask<board_size>();


enum Color : int8_t {
    BLACK = -1, WHITE = 1, EMPTY = 0
};

constexpr Color operator~(Color color) {
    return (color == BLACK) ? WHITE : BLACK;
}



constexpr size_t get_num_fields(size_t board_size) {
    size_t num_squares = board_size * board_size;
    size_t num_fields = num_squares / 64;
    if ((num_squares % 64ull) == 0)
        return
                num_fields;
    else
        return num_fields + 1;
}


#endif //READING_TYPES_H
