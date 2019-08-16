#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <iostream> 
#include <utility> 
#include "enums.h"
#include "board.h"
#include "food.h"
#include "block.h"
#define SNAKE_SIZE 100
#define SNAKE_HEAD_INDEX 0

class Snake
{
private:
	std::vector< std::tuple<int, int, int>> snake_coords;
	int length;
public:
	Snake();
	Snake(std::tuple<int, int, int> coords); 
	~Snake();
	std::vector< std::tuple<int, int, int>> getCoords(void);
	void setInitialCoords(std::tuple<int, int, int> coords);
	bool Move(directions dir, const Board* board, Food* food); 
	bool canEat(std::tuple<int, int, int> head_coords, std::tuple<int, int, int> food_coords);
	bool doesCollide(std::vector<std::tuple<int, int, int>> snake, const Board* board);
	void Draw(void);
};

#endif SNAKE_H