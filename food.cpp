#include "food.h"


//TODO: what kind of position
Food::Food()
{
	Food::block = new Block(glm::vec3(2.0f, 1.0f, 2.0f)); //y ==1
	Food::setBuffers();
}

Food::Food(glm::ivec3 coordinates)
{
	Food::block = new Block(coordinates);
	Food::setBuffers();
}

Food::~Food(){
	delete Food::block;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO); //TODO: CHECK
}

void Food::setCoords(glm::ivec3 coordinates)
{
	Food::block->setCoordinates(coordinates);
}

glm::vec3 Food::getCoords() const
{
	return Food::block->getCoordinates();
}

void Food::getEaten(const Board* board, std::vector<Block*> snake_coords)
{
	Food::setCoords(board->generateFoodPosition(snake_coords));
	//get new coordinates
};


void Food::Draw(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, Shader* shaderProgram)
{
	static glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	static float degrees = 0.0f;
	static bool up = true;
	degrees += 1.0f;
	if (degrees > 360) {
		degrees = 0.0f;
	}
	if (up) {
		translation.y += 0.01f;
	} else {
		translation.y -= 0.01f;
	}
	if (translation.y > 0.5f) {
		up = false;
	}
	else if (translation.y < 0.0f) {
		up = true;
	}
	shaderProgram->setInt("textureChoice", 3);
	glBindVertexArray(Food::VAO);
	block->Draw(model_matrix,view_matrix,projection_matrix, shaderProgram, degrees, translation);
	glBindVertexArray(0);
}


void Food::setBuffers() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//TODO: zaladuj tutaj dane blokow
	glBufferData(GL_ARRAY_BUFFER, sizeof(Food::vertices), Food::vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Food::vertices), Food::vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

