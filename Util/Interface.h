//
// Created by robin on 18.02.22.
//

#ifndef READING_INTERFACE_H
#define READING_INTERFACE_H

#include "../Board.h"
#include "../Search.h"
#include "../NodeAllocator.h"
#include "Util.h"
//some protocol to communicate with a gui

class Interface {

private:
    Search<11> search;

public:

    Interface();

    void clear_board();

    void send_new_move(size_t move);

    void receive_move(size_t move);

    void loop();

    void search_position(int max_time, int max_nodes);
};


#endif //READING_INTERFACE_H
