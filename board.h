#ifndef BOARD_H
#define BOARD_H

#include <glad/glad.h> //TODO:czy glad tez?? 
#include <GLFW\glfw3.h>
//#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include <stdio.h>
#include <vector>
#include <utility>
#include <tuple>
#include "block.h"
#include "enums.h"
#include <cstdlib>

class Board
{
private:
	Gluint VAO, VBO;
	glm::ivec3 dimensions;
	std::vector<Block *> blocks;
public:
	Board();
	Board(glm::ivec3 dimensions);
	~Board();
	void setDimensions(glm::ivec3 dimensions);
	glm::ivec3 getDimensions(void) const;
	glm::ivec3 generateSnakePosition() const;
	glm::ivec3 generateFoodPosition(std::vector<glm::ivec3> snake_coords) const;
	void Draw(); //pobierz koordynaty blokow zeby przeslac je i printowac
};

#endif BOARD_H