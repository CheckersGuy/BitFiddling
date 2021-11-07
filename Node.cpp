//
// Created by robin on 07.11.21.
//

#include "Node.h"

float Node::operator()() const {
    //maximally favour unvisited children
    if (num_visits == 0) {
        return std::numeric_limits<float>::max();
    } else {
        //uct formula to be put here

    }
}

Node *Node::best_child() {
    //for now simply selects the child with the most visits
    Node *max = nullptr;
    uint32_t max_visits = 0;
    for (auto &child: children) {
        if (child->num_visits > max_visits) {
            max_visits = child->num_visits;
            max = child.get();
        }
    }

    return max;
}

Node *Node::select(Prng &generator) {
    //select is trickier since we want to uniformly select
    //from all the nodes with max_value
    //we can find the max_value and at the same time count how many times
    //we have seen it

    size_t num_max = 0;
    float max = std::numeric_limits<float>::min();
    auto uct = [](Node *child) {
        return 0.f;
    };

    for (auto &child: children) {
        float child_uct = uct(child.get());
        if (child_uct > max) {
            max = child_uct;
            num_max = 1;
        } else if (child_uct == max) {
            num_max++;
        }
    }

    //Now we randomly select the child with max_value

    size_t rand_index = generator() % num_max;
    size_t counter = 0;

    for (auto &child: children) {
        float child_uct = uct(child.get());
        if (counter == rand_index) {
            return child.get();
        }
        if (child_uct == max) {
            counter++;
        }
    }


    return nullptr;
}

void Node::back_up(Color result, Color turn) {
    //backup goes here
}
