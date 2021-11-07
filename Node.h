//
// Created by robin on 07.11.21.
//

#ifndef READING_NODE_H
#define READING_NODE_H


#include <cstdint>
#include <c++/10/limits>
#include <memory>
#include <vector>
#include "Prng.h"
#include "Position.h"
class Node {

private:
    Node *parent;
    uint32_t num_visits{0};
    uint32_t num_rave{0};
    float q_value{0};
    float q_rave{0};
    std::vector<std::unique_ptr<Node>> children;

public:


    float operator()() const;

    Node* best_child();

    Node* select(Prng& generator);

    template<size_t board_size> void expand(Position<board_size>& pos){
        //expanding goes here
    }

    void back_up(Color result, Color turn);

};


#endif //READING_NODE_H
