//
// Created by robin on 04.12.21.
//

#ifndef READING_GAMEGEN_H
#define READING_GAMEGEN_H

#include "../Board.h"

template<size_t board_size>
struct Sample{
    float q_value;
    std::array<uint32_t,board_size*board_size>child_visits;
};

class GameGen {

private:
    size_t max_games{0};
    size_t nodes_p_move{50000};
    size_t parallelism{1};

};


#endif //READING_GAMEGEN_H
