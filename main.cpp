#include <iostream>
#include <chrono>
#include "Prng.h"
#include "Position.h"
#include "Union.h"
#include "Board.h"

int main(int argl, const char **argc) {




    Prng generator(231231ull);
    auto t1 = std::chrono::high_resolution_clock::now();
    int sum = 0;
    Board<11> pos;
    for (auto k = 0; k < 1; ++k) {
        auto result = pos.play_out();
        std::cout<<"Result" <<result<<std::endl;
        sum += result;
        std::cout<<pos.get_position()<<std::endl;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto count = (t2 - t1).count();
    std::cout << count / 1000000 << std::endl;
    std::cout << "Summe: " << sum << std::endl;


    std::cout<<"Test: "<<(int)pos.get_union().in_same_set(111,SOUTH<11>)<<std::endl;
    std::cout<<"Test2: "<<(int)pos.get_union().sizes[111]<<std::endl;
    std::cout<<"Test3: "<<(int)pos.get_union().sizes[SOUTH<11>]<<std::endl;
    return 0;
}
