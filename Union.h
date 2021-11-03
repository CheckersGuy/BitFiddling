//
// Created by robin on 24.10.21.
//

#ifndef READING_UNION_H
#define READING_UNION_H

#include <immintrin.h>
#include <array>
template<size_t board_size>
class Union {
private:
    std::array<uint8_t, board_size * board_size> indices;
    std::array<uint8_t, board_size * board_size> sizes{0};

public:

    Union() {
        for (auto i = 0; i < board_size * board_size; ++i) {
            indices[i] = i;
        }
    }

    void merge(size_t rootx, size_t rooty);

    size_t root(size_t idx);

    bool in_same_set(size_t idx, size_t idy);

};


#endif //READING_UNION_H
