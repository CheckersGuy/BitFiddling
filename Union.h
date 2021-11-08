//
// Created by robin on 24.10.21.
//

#ifndef READING_UNION_H
#define READING_UNION_H

#include <immintrin.h>
#include <array>
#include "types.h"
template<size_t board_size> constexpr size_t NORTH = board_size * board_size;

template<size_t board_size> constexpr size_t SOUTH = board_size * board_size + 1;
template<size_t board_size> constexpr size_t WEST = board_size * board_size + 2;
template<size_t board_size> constexpr size_t EAST = board_size * board_size + 3;


template<size_t board_size>
class Union {
public:
    static constexpr size_t NUM_SETS = board_size * board_size + 4;
    std::array<S, NUM_SETS> indices;
    std::array<uint8_t, NUM_SETS> sizes;

public:

    Union() {
        for (size_t i = 0; i < NUM_SETS; ++i) {
            indices[i] = i;
            sizes[i] = 1;
        }

    }

    void merge(size_t x, size_t y) {
        //doing union by size
        auto rootx = root(x);
        auto rooty = root(y);

        if (rootx == rooty)
            return;

        if (sizes[rootx] > sizes[rooty]) {
            indices[rooty] = rootx;
            sizes[rootx] += sizes[rooty];
        } else {
            indices[rootx] = rooty;
            sizes[rooty] += sizes[rootx];

        }
    }

    size_t root(size_t idx) {
        size_t current = idx;
        while (current != indices[current]) {
            indices[current] = indices[indices[current]];
            current = indices[current];
        }
        return current;
    }

    bool in_same_set(size_t idx, size_t idy) {
        return root(idx) == root(idy);
    }

};


#endif //READING_UNION_H
