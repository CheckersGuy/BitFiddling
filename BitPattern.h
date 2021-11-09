//
// Created by robin on 03.11.21.
//

#ifndef READING_BITPATTERN_H
#define READING_BITPATTERN_H

#include <array>
#include "types.h"
#include <iostream>
#include <immintrin.h>

template<size_t board_size>
class bit_pattern;

template<size_t board_size>
struct BitSetIterator {
    bit_pattern<board_size> &bits;
    int field_index{get_num_fields(board_size)};
    uint64_t mask;

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = size_t;
    using pointer = value_type *;
    using reference = value_type &;

    //stuff goes here

    BitSetIterator(const BitSetIterator &other) : bits(other.bits) {
        field_index = other.field_index;
        mask = other.mask;
    }

    explicit BitSetIterator(bit_pattern<board_size> &pattern) : bits(pattern) {
        for (auto i = 0; i < get_num_fields(board_size); ++i) {
            if (pattern.fields[i] != 0) {
                field_index = i;
                break;
            }
        }
        mask = pattern.fields[field_index];
    }

    BitSetIterator &operator++() {
        //removes the lsb
        mask &= mask - 1;
        if (mask == 0 && field_index < get_num_fields(board_size) - 1) {
            field_index++;
            mask = bits.fields[field_index];
        }
        return *this;
    }

    BitSetIterator operator++(int) {
        BitSetIterator next(*this);
        mask &= mask - 1;
        if (mask == 0 && field_index < get_num_fields(board_size) - 1) {
            field_index++;
            mask = bits.fields[field_index];
        }
        return next;
    }

    size_t operator*() {
        //returns the current referenced index
        //this throws an error if applied to end-iterator;
        size_t index = _tzcnt_u64(mask) + 64 * field_index;
        return index;
    }

    bool operator==(BitSetIterator other) {
        return (bits == other.bits && (field_index == other.field_index) && (mask == other.mask));
    }

    bool operator!=(BitSetIterator other) {
        return (bits != other.bits || field_index != other.field_index || mask != other.mask);
    }
};


template<size_t board_size>
struct bit_pattern {
    std::array<uint64_t, get_num_fields(board_size)> fields{0};

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

    friend bit_pattern operator&(bit_pattern &one, bit_pattern &two) {
        bit_pattern<board_size> next;
        constexpr size_t last = get_num_fields(board_size) - 1;
        for (auto i = 0; i < last; ++i) {
            //carefull here, that may not work
            //the last field needs to be handled carefully
            //because we dont want to flip bits beyond board_size*board_size
            next.fields[i] = (one.fields[i] & two.fields[i]);
        }
        next.fields[last] = (one.fields[last] & two.fields[last]);
        next.fields[last] &= LEFT_MASK<board_size>;
        return next;
    }

    bit_pattern &operator&=(bit_pattern &other) {
        constexpr size_t last = get_num_fields(board_size) - 1;
        for (auto i = 0; i < last; ++i) {
            fields[i] = (fields[i] & other.fields[i]);
        }
        fields[last] = (fields[last] & other.fields[last]);
        fields[last] &= LEFT_MASK<board_size>;
        return *this;
    }

    friend bit_pattern operator|(bit_pattern &one, bit_pattern &two) {
        bit_pattern next;
        constexpr size_t last = get_num_fields(board_size) - 1;
        for (auto i = 0; i < last; ++i) {
            next.fields[i] = (one.fields[i] | two.fields[i]);
        }
        next.fields[last] = (one.fields[last] | two.fields[last]);
        next.fields[last] &= LEFT_MASK<board_size>;
        return next;
    }

    bit_pattern &operator|=(bit_pattern &other) {
        constexpr size_t last = get_num_fields(board_size) - 1;
        for (auto i = 0; i < last; ++i) {
            fields[i] = (fields[i] | other.fields[i]);
        }
        fields[last] = (fields[last] | other.fields[last]);
        fields[last] &= LEFT_MASK<board_size>;
        return *this;
    }

    friend bit_pattern operator~(bit_pattern other) {
        bit_pattern next;
        constexpr size_t last = get_num_fields(board_size) - 1;
        for (auto i = 0; i < last; ++i) {
            next.fields[i] = ~(other.fields[i]);
        }
        next.fields[last] = ~(other.fields[last]);
        next.fields[last] &= LEFT_MASK<board_size>;
        return next;
    }

    void print() {
        for (auto i = 0; i < board_size * board_size; ++i) {
            if (is_set(i)) {
                std::cout << "1";
            } else {
                std::cout << "0";
            }
        }
        std::cout << "\n";
    }

    BitSetIterator<board_size> begin() {
        BitSetIterator<board_size> next(*this);
        return next;
    }

    BitSetIterator<board_size> end() {
        BitSetIterator<board_size> next(*this);
        next.field_index = get_num_fields(board_size) - 1;
        next.mask = 0;
        return next;
    }

    bool operator==(bit_pattern other) {
        for (auto i = 0; i < fields.size(); ++i) {
            if (fields[i] != other.fields[i])
                return false;
        }
        return true;
    }

    bool operator!=(bit_pattern other) {
        for (auto i = 0; i < fields.size(); ++i) {
            if (fields[i] != other.fields[i])
                return true;
        }
        return false;
    }

};


#endif //READING_BITPATTERN_H
