#include "light_source.h"

lightSource::lightSource() {
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(0);
}

lightSource::~lightSource(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO); 
}

void lightSource::Draw() {

}