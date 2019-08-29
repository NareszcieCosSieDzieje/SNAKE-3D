#include "block.h"


Block::Block(void)
{
	//smth
}

Block::Block(glm::vec3 coordinates, textures texture)
{
	Block::texture = texture;
	Block::coordinates = coordinates;
	//yay
	//set params
}

Block::Block(glm::vec3 coordinates)
{
	Block::coordinates = coordinates;
	//yay
	//set params
}

Block::~Block()
{
	
}


void Block::setCoordinates(glm::vec3 coordinates) {
	this->coordinates = coordinates;
}

glm::vec3 Block::getCoordinates() {
	return this->coordinates;
}

/*float* block::getvertices() {
	return block::vertices;
}*/


//TODO: czy konstruktor bierze jakies offsety oraz typ tekstury kolor niech bierze /// moze zamiast wskaznikow  = new Block(Snake::snake_coords[i]); // czy tu podac offset
void Block::Draw(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, Shader* shaderProgram)
{
	/*if (Block::texture == board)
	{
		
	}
	else if (Block::texture == snake)
	{
		
	}
	else if (Block::texture == food)
	{
		
	}*/
	model_matrix = glm::translate(model_matrix, Block::coordinates);
	shaderProgram->setMat4("model", model_matrix);
	shaderProgram->setMat4("view", view_matrix);
	shaderProgram->setMat4("projection", projection_matrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Block::Draw(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, Shader* shaderProgram, float rotation_degrees, glm::vec3 translation)
{
	model_matrix = glm::translate(model_matrix, Block::coordinates);
	model_matrix = glm::translate(model_matrix, translation);
	model_matrix = glm::rotate(model_matrix, glm::radians(rotation_degrees), glm::vec3(0.0f, 1.0f, 0.0f)); 
	shaderProgram->setMat4("model", model_matrix);
	shaderProgram->setMat4("view", view_matrix);
	shaderProgram->setMat4("projection", projection_matrix);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
