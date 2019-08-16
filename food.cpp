#include "food.h"

//TODO: what kind of position
Food::Food()
{
	std::get<0>(Food::coords) = 2;
	std::get<1>(Food::coords) = 2;
	std::get<2>(Food::coords) = 0;
}

Food::Food(std::tuple<int, int, int> coords)
{
	Food::setCoords(coords);
}

Food::~Food(){}

void Food::setCoords(std::tuple<int, int,int> coords)
{
	Food::coords = coords;
}

std::tuple<int, int, int> Food::getCoords() const
{
	return Food::coords;
}

void Food::getEaten(const Board* board, std::vector<std::tuple<int, int,int>> snake_coords)
{
	Food::setCoords(board->generateFoodPosition(snake_coords));
	//get new coordinates
};


void Food::Draw()
{
	Block block(Food::coords, food); // i reszta
	block.Draw();
}
