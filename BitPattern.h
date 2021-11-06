//
// Created by robin on 03.11.21.
//

#ifndef READING_BITPATTERN_H
#define READING_BITPATTERN_H
#include <array>

constexpr size_t get_num_fields(size_t board_size) {
    size_t num_squares = board_size * board_size;
    size_t num_fields = num_squares / 64;
    if ((num_squares % 64ull) == 0)
        return
                num_fields;
    else
        return num_fields + 1;
}

template<size_t board_size>
struct bit_pattern {


    static constexpr size_t num_squares = board_size * board_size;
    std::array<uint64_t, get_num_fields(board_size)> fields{0};

    constexpr size_t get_num_squares() {
        return num_squares;
    }

    constexpr void set_bit(size_t idx) {
        size_t field_index = idx / 64ull;
        size_t sub_index = idx & (63ull);
        fields[field_index] |= 1ull << sub_index;
    }

    constexpr void clear_bit(size_t idx) {
        size_t field_index = idx / 64ull;
        size_t sub_index = idx & (63ull);
        fields[field_index] &= ~(1ull << sub_index);
    }

    constexpr bool is_set(size_t idx) {
        size_t field_index = idx / 64ull;
        size_t sub_index = idx & (63ull);
        return (fields[field_index] & (1ull << sub_index)) != 0;
    }
};

#endif //READING_BITPATTERN_H
