#include "board.h"
#include <cstdlib>

// Constructor makes a square g_board from 50x50x1 to 100x100x1
//TODO: Get better randomization + seed in main

Board::Board()
{
	/*
	int random = (rand() % 100) + 50;     //x     y      z
	Board::setDimensions(std::make_tuple(random, 0, random)); //0 or 1
	const int size = std::get<0>(Board::coords)*std::get<1>(coords)*std::get<2>(coords);
	Board::blocks.resize(size);  //TODO: czy parametr z jest okej
	std::tuple<int, int, int> b_coords;
	std::get<0>(b_coords) = 0;
	std::get<1>(b_coords) = 0;
	std::get<2>(b_coords) = 0;
	for(int i = 0;i<2;i++)//height i width a nie jakies 2
	{

		Block b(b_coords, board);
		Board::blocks[i] = b;
		std::get<0>(b_coords) = std::get<0>(b_coords) + 1 ;
		std::get<1>(b_coords) = std::get<1>(b_coords) + 1;
	}
	
	//now set the coords for the blocks and create them
	*/
}

Board::Board(glm::ivec3 dimensions)
{
	Board::setDimensions(dimensions);

	for (int h = 0; h < Board::dimensions.y; h++) {
		for (int w = 0; Board::dimensions.x; w++) {
			for (int l = 0; Board::dimensions.z; l++) {
				Board::blocks.push_back(new Block(/*stuff*/)); //TODO: insert stuff based on dimensions
			}
		}
	}

	//generate vaos

	glGenVertexArrays(1, Board::VAO);
	glGenBuffers(1, Board::VBO); 
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	//TODO: zaladuj tutaj dane blokow
	glBufferData(GL_ARRAY_BUFFER, sizeof( float*verticies), Board::vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); //unbinding 
}

Board::~Board()
{
	//TODO: yall
}


void Board::setDimensions(glm::ivec3 dimensions)
{
	Board::dimensions = dimensions;
}


glm::ivec3 Board::getDimensions() const
{
	return Board::dimensions;
}

//TODO: float to int cast
glm::ivec3 Board::generateSnakePosition() const
{
	glm::ivec3 random;
	random.x = rand() % static_cast<int>(Board::dimensions.x * 0.6f) + static_cast<int>(Board::dimensions.x * 0.4f);
	random.y = 0;//TODO: wysokosc
	random.z = rand() % static_cast<int>(Board::dimensions.z * 0.6f) + static_cast<int>(Board::dimensions.z * 0.4f);
	return random;
}

//Sposob na przyspieszenie? will this work can i make it better tbh?
glm::ivec3 Board::generateFoodPosition(std::vector<glm::ivec3> snake_coords) const
{
	bool found = false;
	bool continuer = false;
	glm::ivec3 random;
	while(!found)
	{
		random.x = rand() % Board::dimensions.x + 0;
		random.y = 0; //czy y serio 0 czy wyzej nizej ustawialny?? / jaka wysokosc boarda???
		random.z = rand() % Board::dimensions.z + 0;
		for (int i = 0; i < snake_coords.size(); i++)
		{
			if (random == snake_coords[i])
			{
				continuer = true;
				break;
			}
		}
		if (continuer)
		{
			continuer = false;
			continue;
		}
		found = true;
	}
	return random;
}


void Board::Draw()
{
	glBindVertexArray(Board::VAO);
	//opengl stuff
	for (auto const& block : Board::blocks) {
		block->Draw();
	}
	glBindVertexArray(0);
}


