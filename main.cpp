#include <iostream>
#include <chrono>
#include <iterator>
#include "Prng.h"
#include "Position.h"
#include "Union.h"
#include "Board.h"
#include "types.h"
#include "Node.h"
#include "Search.h"
int main(int argl, const char **argc) {

/*
    bit_pattern<11>test;
    test.set_bit(10);
    test.set_bit(11);
    test.set_bit(100);
    test.set_bit(101);


    //if the pattern above is empty, iterators do not work anymore
    //that needs to be fixed and may even resolve search issues
    for(auto sq : test){
        std::cout<<sq<<std::endl;
    }




    return 0;
*/


    constexpr size_t board_size =100;
    const size_t max_moves = board_size*board_size;


    Search<board_size> search;
    search.set_max_tree_size(100000000);
    for(auto i=0;i<max_moves;++i){
        if(search.board.get_winner()!=EMPTY)
            break;

        if(search.board.get_position().color == BLACK){
            use_rave= false;
            search.max_time =3000;
            search.set_max_nodes(1000000000);

        }else{
            use_rave=true;
            search.max_time =3000;
            search.set_max_nodes(1000000000);
        }
        std::cout<<"Nodes:"<<search.nodes_in_tree<<std::endl;

        auto t1 = std::chrono::high_resolution_clock::now();
        search.search();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto count = (t2 - t1).count();
        std::cout << "Time: " << count / 1000000 << std::endl;
        auto action = search.get_best_action();
        search.board.make_move(action);
        std::cout<<search.board.get_position()<<std::endl;
        std::cout<<"Action: "<<action<<std::endl;
        std::cout<<search.board.get_position().get_num_empty()<<std::endl;
        std::cout<<"Winner"<<search.board.get_winner()<<std::endl;
        auto pv =search.get_pv();
        std::copy(pv.begin(),pv.end(),std::ostream_iterator<int>(std::cout," "));
        std::cout<<std::endl;
        std::cout<<"QValue: "<<((float)~search.board.get_position().color)*search.root->best_child()->get_win_over_visits()<<std::endl;
        std::cout<<std::endl;
   /*     for(auto&child : search.root->children){
            std::cout<<child->num_visits<<std::endl;
            std::cout<<"Move:" <<child->move<<std::endl;
        }
        auto* node  =search.root->select(gen);
        std::cout<<"Selected: "<<node->num_visits<<std::endl;
*/

    }





    /*   Prng g(321231ull);

           for(auto k=0;k<10000000;++k){
               bit_pattern<11> pattern;
               std::vector<size_t> indices;
               for (auto i = 0; i < 30; ++i) {
                   auto rand = g() % 121;
                   if(std::find(indices.begin(),indices.end(),rand)==indices.end()){
                       indices.emplace_back(rand);
                       pattern.set_bit(rand);
                   }
               }
               std::sort(indices.begin(), indices.end());

               size_t count = 0;
               for (auto sq: pattern) {
                   if(indices[count]!=sq){
                       pattern.print();
                       std::exit(-1);
                   }

                   count++;
               }
           }
   */


    return 0;

/*

    auto t1 = std::chrono::high_resolution_clock::now();
    int sum = 0;
    for (auto k = 0; k < 50000; ++k) {
        Board<11> pos;
        auto result = pos.play_out();
        sum += result;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto count = (t2 - t1).count();
    std::cout << count / 1000000 << std::endl;
    std::cout << "Summe: " << sum << std::endl;

    constexpr size_t board_size = 11;
*/


    return 0;
}
