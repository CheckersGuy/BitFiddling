#include <iostream>
#include <chrono>
#include <iterator>
#include "Prng.h"
#include "Position.h"
#include "Union.h"
#include "Board.h"
#include "types.h"

int main(int argl, const char **argc) {


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
    if constexpr(std::is_same_v<SquareType<11>,uint8_t>){
        std::cout<<"Using uint8t"<<std::endl;
    }else{
        std::cout<<"Using uint16t"<<std::endl;
    }




    return 0;
}
