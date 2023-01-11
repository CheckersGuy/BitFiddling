//
// Created by robin on 24.10.21.
//

#ifndef READING_UNION_H
#define READING_UNION_H

#include <immintrin.h>
#include <array>
#include "types.h"
#include <memory>




class Union {
private:
    const size_t NUM_SETS;
    const size_t size;
    std::unique_ptr<uint16_t[]> indices;
    std::unique_ptr<uint16_t[]> sizes;

public:
    const size_t WEST,NORTH,SOUTH,EAST;
    Union(size_t size): size(size),NUM_SETS(size*size+4),NORTH(size*size),SOUTH(size*size+1),WEST(size*size+2),EAST(size*size+3) {
        indices = std::make_unique<uint16_t[]>(NUM_SETS);
        sizes = std::make_unique<uint16_t[]>(NUM_SETS);
        for (size_t i = 0; i < NUM_SETS; ++i) {
            indices[i] = i;
            sizes[i] = 1;
        }
    }

    Union(const Union &other):size(other.size),NUM_SETS(other.size*other.size+4) ,NORTH(other.size*other.size),SOUTH(other.size*other.size+1),WEST(other.size*other.size+2),EAST(other.size*other.size+3) {
        indices = std::make_unique<uint16_t[]>(NUM_SETS);
        sizes = std::make_unique<uint16_t[]>(NUM_SETS);
        for (size_t i = 0; i < size; ++i) {
            indices[i] = other.indices[i];
            sizes[i] = other.sizes[i];
        }
    }

    Union &operator=(const Union &other) {
        for (size_t i = 0; i < NUM_SETS; ++i) {
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


    bool is_white_win(){
      return in_same_set(WEST,EAST);
    }

    bool is_black_win(){
      return in_same_set(NORTH,SOUTH);
    }

    void merge_south(size_t index){
      merge(SOUTH,index);
    }

  void merge_north(size_t index){
      merge(NORTH,index);
    }

  void merge_east(size_t index){
      merge(EAST,index);
    }

  void merge_west(size_t index){
      merge(WEST,index);
    }


    bool in_same_set(size_t idx, size_t idy) {
        return root(idx) == root(idy);
    }

    void clear(){
       for (size_t i = 0; i < size; ++i) {
            indices[i] = i;
            sizes[i] = 1;
        }

    }

};


#endif //READING_UNION_H
