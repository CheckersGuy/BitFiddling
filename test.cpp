#include <iostream>
#include <array>
#include <immintrin.h>

constexpr size_t get_num_fields(size_t board_size){
	size_t num_squares = board_size*board_size;
	size_t num_fields = num_squares/64;
	if((num_squares&64)==0)
		return num_fields;
	else
		return num_fields+1;
}


constexpr size_t get_leftover_bits(size_t board_size){
	size_t num_squares =board_size*board_size;
	size_t left_over = get_num_fields(board_size)*64-num_squares;
	return left_over;
}

template<size_t board_size> struct bit_pattern{

static constexpr size_t num_squares = board_size*board_size;
std::array<uint64_t,get_num_fields(board_size)>fields{0};

constexpr size_t get_num_squares(){
	return num_squares;
}

constexpr void set_bit(size_t idx){
 size_t field_index = idx/64;
 size_t sub_index = idx % 64;
 fields[field_index] |=1u<<sub_index;

}

constexpr void clear_bit(size_t idx){
size_t field_index = idx%64;
size_t sub_index = idx%64;
fields[field_index]&=~(1u<<sub_index);
}

size_t count_one_bits(){
size_t count =0;
for(auto i=0;i<fields.size();++i){
count+=__builtin_popcountll(fields[i]);
}
return count;
}



};

enum Color{
	BLACK,WHITE
};

constexpr Color operator~(Color color){
return (color == BLACK)?WHITE : BLACK;
}

template<size_t board_size> struct Position{
bit_pattern<board_size>BP;
bit_pattern<board_size>WP;
Color color{BLACK};

template<Color color>
void make_move(size_t idx){
if constexpr(color == BLACK){
BP.set_bit(idx);
}else{
WP.set_bit(idx);
}
}

size_t get_num_empty(){
size_t count =0;
for(auto i=0;i<get_num_fields(board_size);++i){
count+=__builtin_popcountll(~(BP.fields[i]|WP.fields[i]));
}
return count;
}

};


int main(int argl,const char** argc){

bit_pattern<11> pat;
Position<11> pos;
std::cout<<"Pattern: "<<pat.get_num_squares()<<std::endl;
std::cout<<"Num_Fields:"<<get_num_fields(11)<<std::endl;
std::cout<<"Left_Over: "<<get_leftover_bits(11)<<std::endl;
std::cout<<"Num_Fields_Array"<<pat.fields.size()<<std::endl;

pat.set_bit(10);
std::cout<<"Num_One_Bits"<<pat.count_one_bits()<<std::endl;

pos.make_move<BLACK>(10);
std::cout<<"Empty Squares:"<<pos.get_num_empty()<<std::endl;



return 0;
}
