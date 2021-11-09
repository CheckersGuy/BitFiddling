//
// Created by robin on 07.11.21.
//

#ifndef READING_NODE_H
#define READING_NODE_H

#include <math.h>

#include <cstdint>
#include <c++/10/limits>
#include <memory>
#include <vector>
#include "Prng.h"
#include "Position.h"
#include <memory>

template<size_t board_size>
class Node {

public:
    using node_ptr = std::unique_ptr<Node<board_size>>;
    Node *parent;
    uint32_t num_visits{0};
    uint32_t num_rave{0};
    float q_value{0};
    float q_rave{0};
    std::unique_ptr<node_ptr[]> children;
    SquareType<board_size> move;
    SquareType<board_size> num_children{0};

public:

    Node()=default;

    Node(uint16_t move, Node *parent) : move(move), parent(parent) {}

    auto begin() {
        return children.begin();
    }

    auto end() {
        return children.end();
    }


    Node *operator[](int index) {
        return children[index].get();
    }

    void update(float reward) {
        num_visits++;
        q_value += reward;
    }

    uint32_t get_num_visits() {
        return num_visits;
    }

    float get_q() {
        return q_value;
    }

    float get_q_rave() {
        return q_rave;
    }

    uint32_t get_rave_visits() {
        return num_rave;
    }

    uint16_t get_move() {
        return move;
    }

    Node *get_parent() {
        return parent;
    }


    size_t get_num_children() {
        return num_children;
    }

    void expand(Position<board_size> &pos) {
        //expanding goes here
        auto empty_squares = ~(pos.BP | pos.WP);
        children = std::make_unique<node_ptr[]>(pos.get_num_empty());
        for (auto sq: empty_squares) {
            children[num_children++] = std::make_unique<Node>(sq,this);
        }
    }

    float operator()() const {
        //maximally favour unvisited children
        if (num_visits == 0) {
            return 10e20;
        } else {
            //uct formula to be put here
            float p_visits = static_cast<float>(parent->num_visits);
            float uct = q_value / static_cast<float>(num_visits) +
                        2.4f * std::sqrt(std::log(p_visits) / static_cast<float>(num_visits));
            return uct;
        }
    }

    Node *best_child() {
        //for now simply selects the child with the most visits
        Node *max = nullptr;
        uint32_t max_visits = 0;
        for (auto i=0;i<num_children;++i) {
            auto& child = children[i];
            if (child->num_visits > max_visits) {
                max_visits = child->num_visits;
                max = child.get();
            }
        }

        return max;
    }

    Node *select(Prng &generator) {
        //select is trickier since we want to uniformly select
        //from all the nodes with max_value
        //we can find the max_value and at the same time count how many times
        //we have seen it

        size_t num_max = 0;
        float max = std::numeric_limits<float>::lowest();
        for (auto i=0;i<num_children;++i) {
            auto& child = children[i];
            float child_uct = child->operator()();
            if (child_uct > max) {
                max = child_uct;
                num_max = 1;
            } else if (child_uct == max) {
                num_max++;
            }
        }
        //std::cout<<"Max:"<<max<<std::endl;

        //Now we randomly select the child with max_value

        size_t rand_index = generator() % num_max;
        size_t counter = 0;

        for (auto i=0;i<num_children;++i) {
            auto& child = children[i];
            float child_uct = child->operator()();
            if (child_uct == max) {
                if (counter == rand_index) {
                    return child.get();
                }
                counter++;
            }
        }

        return nullptr;
    }

    void back_up(Color result, Color turn) {
        //backup goes here
        float reward = ((result == turn) ? -1.0f : 1.0f);
        Node *current = this;

        while (current != nullptr) {
            current->update(reward);
            turn = ~turn;
            reward = -reward;
            current = current->get_parent();
        }

    }

};


#endif //READING_NODE_H
