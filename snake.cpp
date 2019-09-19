#include "snake.h"

//TODO: INIT VALUES
Snake::Snake()
{
	Snake::snake_blocks.resize(1);//SNAKE_SIZE); 
	//std::fill(Snake::snake_blocks.begin(), Snake::snake_blocks.end(), NULL);
	Block* block = new Block(glm::vec3(2.0f,1.0f,3.0f));
	Snake::snake_blocks[SNAKE_HEAD_INDEX] = block;
	Snake::length = 1;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//TODO: zaladuj tutaj dane blokow
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

Snake::Snake(glm::ivec3 coordinates) //moze vec3 tylko uzyj floor na wartosciach
{
    Snake::snake_blocks.resize(1); // ma nulle wtedy i dupa w does collide
	//std::fill(Snake::snake_blocks.begin(), Snake::snake_blocks.end(), NULL);
	Block* block = new Block(coordinates);
	Snake::snake_blocks[SNAKE_HEAD_INDEX] = block;
	Snake::length = 1;
	//generate vaos
	glGenVertexArrays(1, &VAO); 
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//TODO: zaladuj tutaj dane blokow
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}


Snake::~Snake(){
	//TODO: cleanup memory from vector
	//TODO: check if this works yooo
	for (auto& block : Snake::snake_blocks) {
		delete block;
		block = nullptr;
	}
	Snake::snake_blocks.clear();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO); //TODO: CHECK
}


std::vector<Block*> Snake::getCoords() //FIXME: rework this whatever it is
{
	return Snake::snake_blocks;
}

void Snake::setInitialCoords(glm::ivec3 coordinates) //FIXME: do i need this even??
{
	Snake::snake_blocks[SNAKE_HEAD_INDEX]->setCoordinates(coordinates);
}




//ruch weza moglby byc taki ze ostatni element na miejsce nowe wrzucam ale jak inne tekstury i ksztaly mialyby bloki to to nie dziala
//ruch w osi z
//ROZWazyc ruch dla length 1 i 2 i 3 !!!!!!!!!!!!!!
bool Snake::Move(directions dir, const Board* board, Food* food) //TODO: BED¥ LEAKY PAMIECI PRZEZ OSTATNI ELEMENT SPRAWDZ??
{
	//trzeba zrobic tak ze rusza sie mniej ale nie moze skrecic do poki nie ma calkowitoliczbowej pozycji
	static std::vector<glm::vec3> block_movement;
	static int counter = 0;
	static bool lock = false;
	static bool head_lock = false;
	static directions current_direction;
	static bool add_tail = false;
	float movement = 0.05f;
	int cycles = 1.0f / movement; //20

	glm::vec3 head_coordinates = Snake::snake_blocks[0]->getCoordinates();
	if (counter == 0) {
		block_movement.clear();
		for (int i = Snake::length - 1; i > 0; i--) //musze miec blokade na kazdy element tak ze on bedzie dojezdzal do tego gdzie byl jego poprzednik w cyklu calkowitym
		{
			glm::vec3 predecesor = Snake::snake_blocks[i]->getCoordinates();
			glm::vec3 succesor = Snake::snake_blocks[i - 1]->getCoordinates();
			glm::vec3 difference = glm::vec3((succesor.x - predecesor.x), (succesor.y - predecesor.y), (succesor.z - predecesor.z));
			//podziel elementy przez ilosc cykli
			difference /= 20;  //cycles
			block_movement.insert(block_movement.begin(), difference); //jeszcze dla glowy ???
		//	Snake::snake_blocks[i]->setCoordinates(Snake::snake_blocks[i - 1]->getCoordinates());
		}
	}
	if (lock == false) {
		lock = true; //NEW!
		current_direction = dir;
	}
	if (current_direction == left)
	{
		head_coordinates.x -= movement; //1 normalnie
	}
	else if (current_direction == right)
	{
		head_coordinates.x += movement;
	}
	else if (current_direction == upwards)
	{
		head_coordinates.z -= movement;
	}
	else if (current_direction == downwards)
	{
		head_coordinates.z += movement;
	}
	else {
	//TODO:???
	}
	++counter;
	if (counter == 20) {//cycles here
		head_coordinates = glm::vec3(round(head_coordinates.x), round(head_coordinates.y), round(head_coordinates.z));
	}
	if (add_tail == true) {
		add_tail = false;
		Snake::snake_blocks.insert(Snake::snake_blocks.begin(), new Block(head_coordinates)); //tutaj jakis lock ze tylko glowa sie rusza xd
		Snake::length++; //TODO: jeszcze to sprawdz xd
	}
	else { //blok sie daje jak on wyjdzie?? //dawac cleara na movement przy koncu cyklu
		if (head_lock == false) { 
			//block movement!!!
			//zbierz te kierunki jak sie maja ruszac i potem ich ruszaj o tyle
			
			for (int i = Snake::length - 1; i > 0; i--) //musze miec blokade na kazdy element tak ze on bedzie dojezdzal do tego gdzie byl jego poprzednik w cyklu calkowitym
			{
				Snake::snake_blocks[i]->setCoordinates(Snake::snake_blocks[i]->getCoordinates() + block_movement[i-1]);
			}
			if (counter == 20) {
				for (int i = 0; i < Snake::length; i++) //musze miec blokade na kazdy element tak ze on bedzie dojezdzal do tego gdzie byl jego poprzednik w cyklu calkowitym
				{
					glm::vec3 coords = Snake::snake_blocks[i]->getCoordinates();
					coords = glm::vec3(abs(round(coords.x)), abs(round(coords.y)), abs(round(coords.z))); //albo abs
					Snake::snake_blocks[i]->setCoordinates(coords);
				}
			}
		}
	    if ((head_lock == true) && (counter == 20)) {
			head_lock = false;
		}//cos do zrobienia head_lock false
		Snake::snake_blocks[0]->setCoordinates(head_coordinates);
	}
	if (counter == 20) {//treshhold here
		counter = 0;
		lock = false;
	}
	if (Snake::canEat(Snake::snake_blocks[0]->getCoordinates(), food->getCoords()) ) //predykat ze musi byc jedzenie w srodku boarda i do tego poza koordynatami g_snake'a
	{
		add_tail = true;
		head_lock = true;
		food->getEaten(board, Snake::snake_blocks);
	}
	else
	{
		if (Snake::doesCollide(Snake::snake_blocks, board))
		{
			return true;
		}
	}
	return false;
}

//jakis czit powiadomienia dla foodassw
bool Snake::canEat(glm::vec3 head_coords, glm::vec3 food_coords)
{
	return ( (head_coords.x == food_coords.x) && (head_coords.y == food_coords.y) && (head_coords.z == food_coords.z) );
}

int	Snake::getLength(){ //NEW
	return this->length;
}

//TODO: SPRAWDZ CZY POZA PLANSZE WYCHODZI !!!
bool Snake::doesCollide(std::vector<Block* > snake, const Board* board)
{
	for(int l = 0; l < snake.size(); l++) //nie snake size tylko length
	{
		if ( (snake[l]->getCoordinates().x < 0) || (snake[l]->getCoordinates().z < 0) || (snake[l]->getCoordinates().y < 0) || (snake[l]->getCoordinates().x > board->getDimensions().x - 1 ) || (snake[l]->getCoordinates().z > board->getDimensions().z - 1 ) /*|| sd(snake[l]->getCoordinates().y > board->getDimensions().y - 1)*/ )
		{
			/*printf("\nsnake[%i]->getCoordinates().x = %.f\n", l, snake[l]->getCoordinates().x);
			printf("snake[%i]->getCoordinates().y = %.f\n", l, snake[l]->getCoordinates().y);
			printf("snake[%i]->getCoordinates().z = %.f\n", l, snake[l]->getCoordinates().z);
			printf("board->getDimensions().x - 1 = %i\n", board->getDimensions().x - 1);
			printf("board->getDimensions().z - 1 = %i\n", board->getDimensions().z - 1);
			*/
			return true;
		}
	}
	for (int i = 1; i < snake.size(); i++)
	{
		if (snake[0]->getCoordinates() == snake[i]->getCoordinates())
		{
			/*
			printf("SNAKE == SNAKE situation 2\n");
			printf("snake[0].x = %.f  AND  snake[%i].x = %.f\n", snake[0]->getCoordinates().x ,i, snake[i]->getCoordinates().x);
			printf("snake[0].y = %.f  AND  snake[%i].y = %.f\n", snake[0]->getCoordinates().y, i, snake[i]->getCoordinates().y);
			printf("snake[0].z = %.f  AND  snake[%i].z = %.f\n", snake[0]->getCoordinates().z, i, snake[i]->getCoordinates().z);
			*/
			return true;
		}
	}
	return false;
}


void Snake::Draw(glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 projection_matrix, Shader* shaderProgram)
{
	shaderProgram->setInt("textureChoice", 2);
	glBindVertexArray(Snake::VAO);
	for (int i = 0; i < Snake::length; i++)
	{
		Snake::snake_blocks[i]->Draw2(model_matrix, view_matrix, projection_matrix, shaderProgram);
	}
	glBindVertexArray(0);
}

