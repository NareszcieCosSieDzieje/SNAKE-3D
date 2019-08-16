#include "snake.h"

//TODO: INIT VALUES
Snake::Snake()
{
	std::tuple<int, int, int> initial = std::make_tuple(-1, -1, -1);
	Snake::snake_coords.resize(SNAKE_SIZE); 
	std::fill(Snake::snake_coords.begin(), Snake::snake_coords.end(), initial);
	std::get<0>(Snake::snake_coords[SNAKE_HEAD_INDEX]) = 5; 
	std::get<1>(Snake::snake_coords[SNAKE_HEAD_INDEX]) = 5;
	std::get<2>(Snake::snake_coords[SNAKE_HEAD_INDEX]) = 0;
	Snake::length = 1;
}

Snake::Snake(std::tuple<int, int, int> coords)
{
	std::tuple<int, int, int> initial = std::make_tuple(-1, -1, -1);
	Snake::snake_coords.resize(SNAKE_SIZE);
	std::fill(Snake::snake_coords.begin(), Snake::snake_coords.end(), initial);
	Snake::setInitialCoords(coords);
	Snake::length = 1;
}

Snake::~Snake(){}


std::vector<std::tuple<int, int, int>> Snake::getCoords()
{
	return Snake::snake_coords;
}

void Snake::setInitialCoords(std::tuple<int, int, int> coords)
{
	Snake::snake_coords[SNAKE_HEAD_INDEX] = coords;
}


//ruch w osi z
//ROZWazyc ruch dla length 1 i 2 i 3 !!!!!!!!!!!!!!
bool Snake::Move(directions dir, const Board* board, Food* food) 
{
	const std::tuple<int, int, int> empty = std::make_tuple(-1,-1,-1);
	std::vector<std::tuple<int, int, int>> moved_snake;
	moved_snake.resize(Snake::snake_coords.size());
	std::fill((moved_snake.begin() + Snake::length) , moved_snake.end(), empty);
	for (int i = 0; i < Snake::length - 2; i++) 
	{
		moved_snake[i+1] = Snake::snake_coords[i];
	}
	moved_snake[SNAKE_HEAD_INDEX] = Snake::snake_coords[SNAKE_HEAD_INDEX];
	if (dir == left) 
	{
		std::get<0>(moved_snake[0]) = std::get<0>(Snake::snake_coords[0]) - 1 ;
	}
	else if (dir == right) 
	{
		std::get<0>(moved_snake[0]) = std::get<0>(Snake::snake_coords[0]) + 1;
	}
	else if (dir == upwards)
	{
		std::get<1>(moved_snake[0]) = std::get<1>(Snake::snake_coords[0]) - 1;
	}
	else if (dir == downwards)
	{
		std::get<1>(moved_snake[0]) = std::get<1>(Snake::snake_coords[0]) + 1;
	}
	else
	{//TODO: 
	}
	if (Snake::canEat(moved_snake[0], food->getCoords()) ) //predykat ze musi byc jedzenie w srodku boarda i do tego poza koordynatami g_snake'a
	{
		food->getEaten(board, Snake::snake_coords);
		if( static_cast<int>(Snake::snake_coords.size()) == Snake::length )
		{
			moved_snake.resize(moved_snake.size() * 2);
			std::fill( (moved_snake.begin() + Snake::length + 1) , moved_snake.end(), empty);
		}
		moved_snake[Snake::length] = Snake::snake_coords[Snake::length - 1]; //TODO: BRO OSTATNI ELEMENT
		Snake::snake_coords = moved_snake;
		Snake::length++;
	}
	else
	{
		Snake::snake_coords = moved_snake;
		if (Snake::doesCollide(Snake::snake_coords, board))
		{
			return true;
		}
	}
	return false;
}

//jakis czit powiadomienia dla fooda
bool Snake::canEat(std::tuple<int, int, int> head_coords, std::tuple<int, int, int> food_coords)
{
	return (head_coords == food_coords);
}

//TODO: SPRAWDZ CZY POZA PLANSZE WYCHODZI !!!
bool Snake::doesCollide(std::vector<std::tuple<int, int, int>> snake, const Board* board)
{
	for(int l = 0; l < snake.size(); l++)
	{
		if ( (std::get<0>(snake[l]) < 0) || (std::get<1>(snake[l]) < 0) || (std::get<2>(snake[l]) < 0) || (std::get<0>(snake[l]) > std::get<0>(board->getDimensions()) - 1 ) || (std::get<1>(snake[l]) > std::get<1>(board->getDimensions()) - 1 ) || (std::get<2>(snake[l]) > std::get<2>(board->getDimensions()) - 1))
		{
			return true;
		}
	}
	for (int i = 1; i < snake.size(); i++)
	{
		if (snake[0] == snake[i])
		{
			return true;
		}
	}
	return false;
}


void Snake::Draw()
{
	/*//block array
	const int length = Snake::length;
	Block blocks[length]; //TODO: czy ti byl length cyz co ja uw=zywa³em
	//stworz koordynaty blokow bloki z offsetem
	for(int i = 0; i < Snake::length; i++)
	{
		Block block(Snake::snake_coords[i]); //TODO: czy konstruktor bierze jakies offsety oraz typ tekstury kolor niech bierze /// moze zamiast wskaznikow  = new Block(Snake::snake_coords[i]); // czy tu podac offset
		blocks[i] = block;
		//block bierze tego tupla jako const nie moze go zmienic i na podstawie tych koordynatow 
		//oblicza sobie pozycje bloku 
	}

	// po stworzeniu blokow namaluj
	//jakies 
	for (int i = 0; i < Snake::length; i++)
	{
		blocks[i].Draw(); //cos takiego?
	}*/

}

