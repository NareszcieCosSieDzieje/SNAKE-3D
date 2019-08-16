#ifndef FOOD_H
#define FOOD_H

#include <utility>
#include <tuple>
#include "board.h"
#include "Block.h"


//#define triple std::tuple<int,int,int>
//template<typename T1, typename T2, typename T3>
//using triple = std::tuple<T1, T2, T3>;

//triple a;

class Food
{
private:
	std::tuple<int, int, int> coords;
public:
	Food();
	Food(std::tuple<int, int, int> coords);
	~Food();
	void setCoords(std::tuple<int, int, int> coords);
	std::tuple<int, int, int> getCoords(void) const;
	void getEaten(const Board* board, std::vector<std::tuple<int, int, int>> snake_coords);
	void Draw(); //const
};

#endif FOOD_H