//
// Created by robin on 24.10.21.
//

#ifndef READING_UNION_H
#define READING_UNION_H

#include <immintrin.h>
#include <array>
#include "types.h"

template<size_t board_size>
class Union {
public:
    static constexpr size_t NUM_SETS = board_size * board_size + 4;
    /* std::array<SquareType<board_size>, NUM_SETS> indices;
     std::array<SquareType<board_size>, NUM_SETS> sizes;*/
    size_t size;
    std::unique_ptr<uint16_t[]> indices;
    std::unique_ptr<uint16_t[]> sizes;


public:

    Union() {
        size = board_size * board_size + 4;
        indices = std::make_unique<uint16_t[]>(size);
        sizes = std::make_unique<uint16_t[]>(size);
        for (size_t i = 0; i < size; ++i) {
            indices[i] = i;
            sizes[i] = 1;
        }
    }

    Union(const Union &other) {
        size = other.size;
        indices = std::make_unique<uint16_t[]>(size);
        sizes = std::make_unique<uint16_t[]>(size);
        for (size_t i = 0; i < size; ++i) {
            indices[i] = other.indices[i];
            sizes[i] = other.sizes[i];
        }
    }

    Union &operator=(const Union &other) {
        size = other.size;
        indices = std::make_unique<uint16_t[]>(size);
        sizes = std::make_unique<uint16_t[]>(size);
        for (size_t i = 0; i < size; ++i) {
            indices[i] = other.indices[i];
            sizes[i] = other.sizes[i];
        }
        return *this;
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
