//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // gets rid of the command line

#include <glad/glad.h>
#include <GLFW\glfw3.h>

//https://github.com/vallentin/glText
#define GLT_IMPLEMENTATION
#include "gltext.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <chrono>
#include <string>
#include <iostream>
#include <thread>
//MATH
#include "glm/glm.hpp"
#include "glm/ext.hpp"

//LOCAL
#include "enums.h"
#include "board.h"
#include "snake.h"
#include "food.h"
#include "shader_loader.h"

//TEXTURES
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include "include\irrKlang.h"
//#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

//TEXT
#include "gltext.h"

//Game Globals
bool game_over;
directions snake_directions;
options game_option;
Board* game_board;
Snake* game_snake;
Food* game_food;
int key_setup;

// settings
unsigned int SCR_WIDTH = 1000; // get from windows?
unsigned int SCR_HEIGHT = 1000;

//GLFW
GLFWwindow* window;

//Shaders
Shader *mainShader;

//OPENGL VARIABLES
GLuint vertexShader, fragmentShader, shaderProgram,
texture1, texture2, texture3, texture4;

//GLM MATRICES
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

//DRAWING TEXT 
bool show_text = true;
GLTtext *text[4];

//float mixer = 0.5f;

//TODO: opengl stuff
void monitor_callback(GLFWmonitor* monitor, int event);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void update(void);
void displayInit(GLTtext* text1, GLTtext* text2, GLfloat x, GLfloat y, GLfloat scale);
void display(void);
void display(bool shouldDisplay, GLTtext* text, GLfloat x = 15.0f, GLfloat y = 10.0f, GLfloat scale = 5.0f);
void displayText(bool shouldDisplay, GLTtext* text, GLfloat x, GLfloat y, GLfloat scale);
void handle_options(void);
void initialize(void);
void freeResources(void);


int main()
{
	srand(time(NULL));

	initialize();

	double accumulator = 0;
	double difference = 0;
	double text_difference = 0;
	std::chrono::steady_clock::time_point last, current, text_last, text_now;
	//Tworzenie tekstu
	text[0] = gltCreateText();
	text[1] = gltCreateText();
	text[2] = gltCreateText();
	text[3] = gltCreateText();

	gltSetText(text[0], "Press Space to start the game!");
	gltSetText(text[1], "Nice!");
	gltSetText(text[2], "To move the snake use the arrow keys.");
	gltSetText(text[3], "Press ENTER to start.\nPress ESC to quit.");

	int numText = 2;
	float w_offset = 0.9;
	float text_scale = 1.0;
	text_last = std::chrono::steady_clock::now();
	last = std::chrono::steady_clock::now();
	while (!glfwWindowShouldClose(window) && (glfwGetKey(window, GLFW_KEY_ENTER) != GLFW_PRESS) ) {
		text_now = std::chrono::steady_clock::now();
		text_difference = (static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(text_now - text_last).count()));
		if ( (text_difference > 5.0) && (numText == 2) ) {
			gltSetText(text[2], "To move the camera use AWSD.");
			numText++;
			w_offset = 4.0;
		}
		else if ( (text_difference > 10.0) && (numText == 3) ){
			gltSetText(text[2], "To change camera angles use UHJK.");
			w_offset = 1.5;
			numText++;
		} 
		else if ((text_difference > 15.0) && (numText == 4)) {
			w_offset = 0.9;
			numText = 2;
			text_last = std::chrono::steady_clock::now();
			gltSetText(text[2], "To move the snake use the arrow keys.");
			//break;
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			game_over = true;
			break;
		}
		current = std::chrono::steady_clock::now();
		difference = (static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(current - last).count()));
		last = std::chrono::steady_clock::now();
		accumulator += difference;
		if ( (0.001 * accumulator) > (1.0/60.0) ) {
		processInput(window);
		accumulator = 0;
		}
		displayInit(text[2], text[3], w_offset*SCR_WIDTH/8, (SCR_HEIGHT - SCR_HEIGHT/6), 4.0f);
	}
	/*
	while (!glfwWindowShouldClose(window) && (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS) && (!game_over) ) {
		processInput(window);
		display(true,text[0], 15.0f, 10.0f, 5.0f);
	}*/
	//show_text = false;

	//points based on time

	//punkty to apples*100/czas gry czas gry to mousi byc miedzy zbieraniem jablek
	
	
	int apple_number = game_snake->getLength() - 1;
	int win_points = (game_board->getDimensions().x * game_board->getDimensions().z)/5;
	//int points = apple_number*100
	std::string apple_text = "Apples: " + std::to_string(apple_number);
	gltSetText(text[0], apple_text.c_str());
	last = std::chrono::steady_clock::now();
	while (!glfwWindowShouldClose(window) && (!game_over) && (apple_number != win_points) )
	{
		apple_number = game_snake->getLength() - 1;
		apple_text =  std::to_string(win_points-apple_number) + " apples to win";
		gltSetText(text[0], apple_text.c_str());
		processInput(window); //TODO:: INPUT ZA CZESTO PROBKOWANY I MOZNA SIE COFAC !!!! przyklad mamy left szybkie up i od razu right i sie cofamy, trzeba 
		//zapamietywac staticiem i mierzyc zeby sie ruszyl jeden blok
		current = std::chrono::steady_clock::now();
		difference = (static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(current - last).count()));
		last = std::chrono::steady_clock::now();
		//printf(" last_time = %.f\nnow_time = %.f\ndifference = %.f", last_time, now_time, accumulator);
		accumulator += difference;
		//printf("accumulator = %.f", accumulator);
		//while (accumulator > (1.0f)) { // by�o (1.0f / 61.0f)
		if ( (0.001 * accumulator) > (1.0/120.0) ) {
		update();
		accumulator = 0;
		}
		//	accumulator -= (1.0f); //by�o 1.0f / 59.0f
		//	if (accumulator < 0) accumulator = 0;
		display(true,text[0], 20.0f, 10.0f, 5.0f);
	}
	//if esc pomin to
	std::string ending_text;
	float end_duration = 1.0;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		ending_text = "CLOSING GAME!";
		gltSetText(text[0], ending_text.c_str());
	}
	else {
		if (apple_number == win_points) {
			ending_text = "YOU WIN!\nAPPLES: " + std::to_string(win_points);
			end_duration = 3.0;
		}
		else {
			ending_text = "GAME OVER!\nTRY AGAIN!";
			end_duration = 2.5;
			
		}
		for (int end_snake = 0; end_snake < 20; end_snake++) { //to ogolnie ma pokazac ze snake wyjechal poza plansze, piekne nie jest ale robi robote
			update();
			//TODO: lepsza plynnosc 
			std::this_thread::sleep_for(std::chrono::milliseconds(12));
		}
		gltSetText(text[0], ending_text.c_str());
	}
	last = std::chrono::steady_clock::now();
	while (true) {
		current = std::chrono::steady_clock::now();
		processInput(window);
		display(true,text[0]);
		difference = static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(current - last).count());
		if (difference > end_duration) {
			break;
		}
	}
	//TODO: przezucic do freeResources
	gltDeleteText(text[0]);
	gltDeleteText(text[1]);
	gltDeleteText(text[2]);
	gltDeleteText(text[3]);
	freeResources();
	return 0;
}

void freeResources() {
	// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
	glDeleteTextures(1, &texture1);
	glDeleteTextures(1, &texture2);
	glDeleteTextures(1, &texture3);
	glDeleteTextures(1, &texture4);


	// Destroy glText
	gltTerminate(); //TODO: NEW!
	glfwTerminate();
}

void initialize()
{
	key_setup = 0;
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//monitors
	int monitor_count;
	GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
	GLFWmonitor* primary = NULL;
	GLFWmonitor* secondary = NULL;
	//jak null to error
	if (monitors == NULL) {
		//error //zle cos
	}
	else {
		primary = monitors[0];
		if (monitor_count > 1) {
			secondary = monitors[1];
		}
	}
	// TODO: usun 
	std::cout << "Monitor count : " << monitor_count;
	//chyba nie uzywac wogole GLFWmonitorfun monitor_callback; //czy uzywac
	//TODO: czy uzywac wogole
	//glfwSetMonitorCallback(monitor_callback);



	// glfw window creation
// --------------------
	const GLFWvidmode* primary_mode;
	const GLFWvidmode* secondary_mode;
	if (monitor_count < 2) {
		primary_mode = glfwGetVideoMode(primary);
		SCR_WIDTH = primary_mode->width;
		SCR_HEIGHT = primary_mode->height;
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Snake-3D", primary, NULL); //monitors[0]
	}
	else {
		secondary_mode = glfwGetVideoMode(secondary);
		SCR_WIDTH = secondary_mode->width;
		SCR_HEIGHT = secondary_mode->height;
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Snake-3D", secondary, NULL); //monitors[1]
	}
	//window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Snake-3D", NULL, NULL); 
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		//return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		//return -1;
	}
	//=====================================TEST===============================================================================================
	// build and compile our shader program
	// ------------------------------------
	// vertex shader
	//vertexShader = loadShader("vertex_shader.glsl");

	

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); 
	unsigned char* data = stbi_load("textures/mercury3.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("textures/green.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);;
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("textures/apple.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);



	mainShader = new Shader("vertex_shader.glsl", "fragment_shader.glsl");
	mainShader->use();
	//czy to wszystko?
	mainShader->setInt("Texture1", 0); //texture stuff
	mainShader->setInt("Texture2", 1);
	mainShader->setInt("Texture3", 2);


	game_over = false;
	snake_directions = right;
	game_board = new Board(glm::ivec3( rand() % 5 + 10, 1, rand() % 5 + 10)); //TODO: parametry
	game_snake = new Snake(); //ssgame_board->generateSnakePosition());
	game_food = new Food(); // game_board->generateFoodPosition(game_snake->getCoords()));


	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3( -1.0f *(game_board->getDimensions().x/2), 3.0f, (-2.5f*game_board->getDimensions().z) ));
	view = glm::rotate(view, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));	
	projection = glm::perspective(glm::radians(45.0f), (float(SCR_WIDTH) / float(SCR_HEIGHT)), 0.1f, 100.0f);


	//Drawing stuff
	gltInit(); //TODO: NEW!!!
}


//TODO: CZASOWE OGARNICZENIE NIE DZIA�A!
void processInput(GLFWwindow* window)
{
	directions snake;
	static bool once = true;
	static std::chrono::steady_clock::time_point last;
	std::chrono::steady_clock::time_point current;
	double difference = 0;
	static float pitch = 45;
	static float yaw = 45;
	float rotation_angle = 0.5;
	float up_constraint = 85;
	float down_constraint = 10;
	if (key_setup == 0) {
		if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (snake_directions != downwards)) {
			snake_directions = upwards;
		}
		else if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (snake_directions != upwards)) {
			snake_directions = downwards;
		}
		else if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && (snake_directions != left)) {
			snake_directions = right;
		}
		else if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && (snake_directions != right)) {
			snake_directions = left;
		}
	}
	else if (key_setup == 1) {
		if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (snake_directions != left)) {
			snake_directions = right;
		}
		else if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (snake_directions != right)) {
			snake_directions = left;
		}
		else if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && (snake_directions != upwards)) {
			snake_directions = downwards;
		}
		else if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && (snake_directions != downwards)) {
			snake_directions = upwards;
		}
	}
	else if (key_setup == 2) {
		if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (snake_directions != upwards)) {
			snake_directions = downwards;
		}
		else if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (snake_directions != downwards)) {
			snake_directions = upwards;
		}
		else if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && (snake_directions != right)) {
			snake_directions = left;
		}
		else if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && (snake_directions != left)) {
			snake_directions = right;
		}
	}
	else if (key_setup == 3) {
		if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (snake_directions != right)) {
			snake_directions = left;
		}
		else if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (snake_directions != left)) {
			snake_directions = right;
		}
		else if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && (snake_directions != downwards)) {
			snake_directions = upwards;
		}
		else if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && (snake_directions != upwards)) {
			snake_directions = downwards;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		if (key_setup == 0) {
			if ((pitch > down_constraint)) {
				view = glm::rotate(view, glm::radians(-rotation_angle), glm::vec3(1.0f, 0.0f, 0.0f));
				pitch -= rotation_angle;
			}
			if (pitch < down_constraint) {
				pitch = down_constraint;
			}
		}
		else if (key_setup == 1) {
			if ((yaw > down_constraint)) {
				view = glm::rotate(view, glm::radians(-rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
				yaw -= rotation_angle;
			}
			if (yaw < down_constraint) {
				yaw = down_constraint;
			}
		}
		else if (key_setup == 2) {
			if ((pitch < up_constraint)) {
				view = glm::rotate(view, glm::radians(rotation_angle), glm::vec3(1.0f, 0.0f, 0.0f));
				pitch += rotation_angle;
			}
			if (pitch > up_constraint) {
				pitch = up_constraint;
			}
		}
		else if (key_setup == 3) {
			if ((yaw < up_constraint)) {
				view = glm::rotate(view, glm::radians(rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
				yaw += rotation_angle;
			}
			if (yaw > up_constraint) {
				yaw = up_constraint;
			}
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		if (key_setup == 0) {
			if ((pitch < up_constraint)) {
				view = glm::rotate(view, glm::radians(rotation_angle), glm::vec3(1.0f, 0.0f, 0.0f));
				pitch += rotation_angle;
			}
			if (pitch > up_constraint) {
				pitch = up_constraint;
			}
		}
		else if (key_setup == 1) {
			if ((yaw < up_constraint)) {
				view = glm::rotate(view, glm::radians(rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
				yaw += rotation_angle;
			}
			if (yaw > up_constraint) {
				yaw = up_constraint;
			}
		}
		else if (key_setup == 2) {
			if ((pitch > down_constraint)) {
				view = glm::rotate(view, glm::radians(-rotation_angle), glm::vec3(1.0f, 0.0f, 0.0f));
				pitch -= rotation_angle;
			}
			if (pitch < down_constraint) {
				pitch = down_constraint;
			}
		}
		else if (key_setup == 3) {
			if (yaw < up_constraint) {
				view = glm::rotate(view, glm::radians(-rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f));
				yaw -= rotation_angle;
			}
			if (yaw < down_constraint) {
				yaw = down_constraint;
			}
		}	
	}
	if (once) {
		last = std::chrono::steady_clock::now();
		once = false;
	}
	current = std::chrono::steady_clock::now();
	difference = (static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(current - last).count()));
	if (difference > 150) {
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			if (key_setup == 0) {
				key_setup = 3;
				view = glm::rotate(view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
				yaw = pitch;
				view = glm::translate(view,
					glm::vec3(1.0f * (game_board->getDimensions().x / 2), -3.0f, (2.5f * game_board->getDimensions().z)));
				view = glm::rotate(view, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				view = glm::translate(view,
				glm::vec3(-2.5f * (game_board->getDimensions().x), 3.0f, (-1.0f *game_board->getDimensions().z/2)) );
				view = glm::rotate(view, glm::radians(-yaw), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			else if (key_setup == 1) {
				key_setup--;
				view = glm::rotate(view, glm::radians(-yaw), glm::vec3(0.0f, 0.0f, 1.0f));
				pitch = yaw;
				view = glm::translate(view,
					glm::vec3(-2.0f * (game_board->getDimensions().x), 3.25f, (1.0f * game_board->getDimensions().z / 2)));
				view = glm::rotate(view, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				view = glm::translate(view,
					glm::vec3(-1.0f * (game_board->getDimensions().x / 2), 3.0f, (-2.5f * game_board->getDimensions().z)));
				view = glm::rotate(view, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else if (key_setup == 2) {
				key_setup--;
				view = glm::rotate(view, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
				yaw = pitch;
				view = glm::translate(view,
					glm::vec3(1.0f * (game_board->getDimensions().x / 2), 3.0f, (-2.0f * game_board->getDimensions().z)));
				view = glm::rotate(view, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				view = glm::translate(view,
					glm::vec3(2.0f * (game_board->getDimensions().x), -3.0f, (-1.0f * game_board->getDimensions().z / 2)));
				view = glm::rotate(view, glm::radians(yaw), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			else if (key_setup == 3) {
				key_setup--;
				view = glm::rotate(view, glm::radians(yaw), glm::vec3(0.0f, 0.0f, 1.0f));
				pitch = yaw;
				view = glm::translate(view,
					glm::vec3(2.5f * (game_board->getDimensions().x), -3.0f, (1.0f * game_board->getDimensions().z / 2)));
				view = glm::rotate(view, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				view = glm::translate(view,
					glm::vec3(-1.0f * (game_board->getDimensions().x/2), -3.0f, (2.0f * game_board->getDimensions().z)));
				view = glm::rotate(view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			if (key_setup == 3) {
				key_setup = 0;
				view = glm::rotate(view, glm::radians(yaw), glm::vec3(0.0f, 0.0f, 1.0f));
				pitch = yaw; 
				view = glm::translate(view,
					glm::vec3(2.5f * (game_board->getDimensions().x), -3.0f, (1.0f * game_board->getDimensions().z / 2)));
				view = glm::rotate(view, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				view = glm::translate(view,
					glm::vec3(-1.0f * (game_board->getDimensions().x / 2), 3.0f, (-2.5f * game_board->getDimensions().z)));
				view = glm::rotate(view, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else if (key_setup == 0) {
				key_setup++;
				view = glm::rotate(view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
				yaw = pitch;
				view = glm::translate(view,
					glm::vec3(1.0f * (game_board->getDimensions().x / 2), -3.0f, (2.5f * game_board->getDimensions().z)));
				view = glm::rotate(view, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				view = glm::translate(view,
					glm::vec3(2.0f * (game_board->getDimensions().x), -3.25f, (-1.0f * game_board->getDimensions().z / 2)));
				view = glm::rotate(view, glm::radians(yaw), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			else if (key_setup == 1) {
				key_setup++;
				view = glm::rotate(view, glm::radians(-yaw), glm::vec3(0.0f, 0.0f, 1.0f));
				pitch = yaw;
				view = glm::translate(view,
					glm::vec3(-2.0f * (game_board->getDimensions().x), 3.0f, (1.0f * game_board->getDimensions().z / 2)));
				view = glm::rotate(view, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				view = glm::translate(view,
					glm::vec3(-1.0f * (game_board->getDimensions().x / 2), -3.0f, (2.0f * game_board->getDimensions().z)));
				view = glm::rotate(view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			else if (key_setup == 2) {
				key_setup++;
				view = glm::rotate(view, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
				yaw = pitch;
				view = glm::translate(view,
					glm::vec3(1.0f * (game_board->getDimensions().x / 2), 3.0f, (-2.0f * game_board->getDimensions().z)));
				view = glm::rotate(view, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				view = glm::translate(view,
					glm::vec3(-2.5f * (game_board->getDimensions().x), 3.0f, (-1.0f * game_board->getDimensions().z / 2)));
				view = glm::rotate(view, glm::radians(-yaw), glm::vec3(0.0f, 0.0f, 1.0f));
			}
		}
		std::cout << "pitch: " << pitch << "\nyaw: " << yaw << std::endl;
		last = std::chrono::steady_clock::now();
	}
	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (key_setup == 0) {
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.1f));
		}
		else if (key_setup == 1) {
			view = glm::translate(view, glm::vec3(-0.1f, 0.0f, 0.0f));
		}
		else if (key_setup == 2) {
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.1f));
		}
		else if (key_setup == 3) {
			view = glm::translate(view, glm::vec3(0.1f, 0.0f, 0.0f));
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (key_setup == 0) {
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.1f));
		}
		else if (key_setup == 1) {
			view = glm::translate(view, glm::vec3(0.1f, 0.0f, 0.0f));
		}
		else if (key_setup == 2) {
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.1f));
		}
		else if (key_setup == 3) {
			view = glm::translate(view, glm::vec3(-0.1f, 0.0f, 0.0f));
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		view = glm::translate(view, glm::vec3(0.0f, -0.1f, 0.0f));
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		view = glm::translate(view, glm::vec3(0.0f, 0.1f, 0.0f));
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void update()
{
	game_over = game_snake->Move(snake_directions, game_board, game_food);
	//if g_food eaten g_food = new Food();
	//menu tutaj?
	//handle_options();
}

void displayInit(GLTtext* text1, GLTtext* text2, GLfloat x, GLfloat y, GLfloat scale)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Ustawienia t�a
	float changingColour = ((sin(glfwGetTime()) / (8 / 3)) + (3 / 8));
	//glClearColor( ((sin(glfwGetTime()) / 2.0f) + 0.5f), 1.0f - ((sin(glfwGetTime()) / 2.0f) + 0.5f) , 1.0f, 1.0f); //tutaj zmienna do zmiany koloru t�a + jakisVariable
	glClearColor(changingColour, changingColour, changingColour, 1.0f); //tutaj zmienna do zmiany koloru t�a + jakisVariable
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture3);

	mainShader->use();
	
	game_board->Draw(model, view, projection, mainShader);

	game_food->Draw(model, view, projection, mainShader);

	game_snake->Draw(model, view, projection, mainShader);
	
	displayText(true, text1, x, y, scale);
	displayText(true, text2, 0.0f, 0.0f, 5.0f);
	
	glfwSwapBuffers(window);
	glfwPollEvents(); //TODO: whats up with this xd

}


void display()
{
	//TODO: usun
	//std::chrono::steady_clock::time_point last, current;
	//double difference = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Ustawienia t�a
	glClearColor(0.5f, 0.5f, 0.0f, 1.0f); //tutaj zmienna do zmiany koloru t�a + jakisVariable
	glClear(GL_COLOR_BUFFER_BIT);
	// Bindowanie tekstur
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture3);

	// W��czenie odpowiedniego shaderProgramu
	mainShader->use();
	//mainShader->setFloat("texMix", mixer); //zmienne uniform shadera

	//local space -> world space -> view space -> clip space -> screen space

	game_board->Draw(model, view, projection, mainShader);
	
	game_food->Draw(model, view, projection, mainShader);
	
	//current = std::chrono::steady_clock::now();
	game_snake->Draw(model, view, projection, mainShader);
	//last = std::chrono::steady_clock::now();
	//difference = (static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(last - current).count()));
	//printf(" game_snake->Draw = %.f\n\n", difference);

	/* 
	if (show_text == true) {
		// Begin text drawing (this for instance calls glUseProgram)
		gltBeginDraw();
		// Draw any amount of text between begin and end
		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		gltDrawText2D(text, 15.0f, 10.0f, 5);
		// Finish drawing text
		gltEndDraw();
	}*/
	glfwSwapBuffers(window);
	glfwPollEvents(); //TODO: whats up with this xd
}

void display(bool shouldDisplay, GLTtext* text, GLfloat x, GLfloat y, GLfloat scale)
{
	//TODO: usun
	//std::chrono::steady_clock::time_point last, current;
	//double difference = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Ustawienia t�a
	glClearColor(0.5f, 0.5f, 0.0f, 1.0f); //tutaj zmienna do zmiany koloru t�a + jakisVariable
	glClear(GL_COLOR_BUFFER_BIT);
	// Bindowanie tekstur
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture3);

	// W��czenie odpowiedniego shaderProgramu
	mainShader->use();
	//mainShader->setFloat("texMix", mixer); //zmienne uniform shadera

	//local space -> world space -> view space -> clip space -> screen space

	game_board->Draw(model, view, projection, mainShader);

	game_food->Draw(model, view, projection, mainShader);

	//current = std::chrono::steady_clock::now();
	game_snake->Draw(model, view, projection, mainShader);
	//last = std::chrono::steady_clock::now();
	//difference = (static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(last - current).count()));
	//printf(" game_snake->Draw = %.f\n\n", difference);

	/*
	if (show_text == true) {
		// Begin text drawing (this for instance calls glUseProgram)
		gltBeginDraw();
		// Draw any amount of text between begin and end
		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		gltDrawText2D(text, 15.0f, 10.0f, 5);
		// Finish drawing text
		gltEndDraw();
	}*/
	displayText(shouldDisplay, text, x, y, scale);

	glfwSwapBuffers(window);
	glfwPollEvents(); //TODO: whats up with this xd
}


void displayText(bool shouldDisplay,GLTtext* text, GLfloat x, GLfloat y, GLfloat scale) {

	if (shouldDisplay) {
		// Begin text drawing (this for instance calls glUseProgram)
		gltBeginDraw();
		// Draw any amount of text between begin and end
		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		gltDrawText2D(text, x, y, scale);
		// Finish drawing text
		gltEndDraw();
	}
}

void handle_options()
{

}

void monitor_callback(GLFWmonitor* monitor, int event)
{
	if (event == GLFW_CONNECTED)
	{
		// The monitor was connected
	}
	else if (event == GLFW_DISCONNECTED)
	{
		// The monitor was disconnected
	}
}