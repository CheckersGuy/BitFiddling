#include "Union.h"

template<size_t board_size>
void Union<board_size>::merge(size_t x, size_t y) {
    //doing union by size

    size_t rootx = root(x);
    size_t rooty = root(y);
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

template<size_t board_size>
size_t Union<board_size>::root(size_t idx) {
    size_t current = idx;
    while (current != indices[current]) {
        indices[current] = indices[indices[current]];
        current = indices[current];
    }
    return current;
}

template<size_t board_size>
bool Union<board_size>::in_same_set(size_t idx, size_t idy) {
    return root(idx) == root(idy);
}
