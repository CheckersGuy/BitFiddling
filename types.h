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

template<size_t board_size> using SquareType =std::conditional_t<board_size <= 15, uint8_t , uint16_t>;

#endif //READING_TYPES_H
