#include "block.h"


Block::Block(void)
{
	
}

Block::Block(glm::vec3 coordinates, textures texture)
{
	
	//yay
	//set params
}

Block::~Block()
{
	
}


//TODO: czy konstruktor bierze jakies offsety oraz typ tekstury kolor niech bierze /// moze zamiast wskaznikow  = new Block(Snake::snake_coords[i]); // czy tu podac offset
void Block::Draw()
{
	if (Block::texture == board)
	{
		
	}
	else if (Block::texture == snake)
	{
		
	}
	else if (Block::texture == food)
	{
		
	}
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
