//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // gets rid of the command line

#include <glad/glad.h>
#include <GLFW\glfw3.h>

//https://github.com/vallentin/glText
#define GLT_IMPLEMENTATION
#include "gltext.h"

#include <chrono>
#include <string>
#include <iostream>

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

//TEXT
#include "gltext.h"

//Game Globals
bool game_over;
directions snake_directions;
options game_option;
Board* game_board;
Snake* game_snake;
Food* game_food;


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
GLTtext *text[3];

float mixer = 0.5f;

//TODO: opengl stuff
void monitor_callback(GLFWmonitor* monitor, int event);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void update(void);
void display(void);
void displayText(GLTtext* text, GLfloat x = 15.0f , GLfloat y = 10.0f , GLfloat scale = 5.0f);
void handle_options(void);
void initialize(void);
void freeResources(void);


int main()
{
	srand(time(NULL));

	initialize();

	double accumulator = 0;
	double difference = 0;
	std::chrono::steady_clock::time_point last, current;
	last = std::chrono::steady_clock::now();
	//Tworzenie tekstu
	text[0] = gltCreateText();
	text[1] = gltCreateText();
	gltSetText(text[0], "Press Space to start the game!");
	while (!glfwWindowShouldClose(window) && (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS) ) {
		processInput(window);
		display();
	}
	//show_text = false;

	//points based on time

	//punkty to apples*100/czas gry czas gry to mousi byc miedzy zbieraniem jablek
	
	
	int apple_number = game_snake->getLength() - 1;
	//int points = apple_number*100
	std::string apple_text = "Apples: " + std::to_string(apple_number);
	gltSetText(text[0], apple_text.c_str());
	while (!glfwWindowShouldClose(window) && (!game_over))
	{
		apple_number = game_snake->getLength() - 1;
		apple_text = "Apples: " + std::to_string(apple_number);
		gltSetText(text[0], apple_text.c_str());
		processInput(window); //TODO:: INPUT ZA CZESTO PROBKOWANY I MOZNA SIE COFAC !!!! przyklad mamy left szybkie up i od razu right i sie cofamy, trzeba 
		//zapamietywac staticiem i mierzyc zeby sie ruszyl jeden blok
		current = std::chrono::steady_clock::now();
		difference = (static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(current - last).count()));
		last = std::chrono::steady_clock::now();
		//printf(" last_time = %.f\nnow_time = %.f\ndifference = %.f", last_time, now_time, accumulator);
		accumulator += difference;
		//printf("accumulator = %.f", accumulator);
		//while (accumulator > (1.0f)) { // by³o (1.0f / 61.0f)
		if ( (0.001f * accumulator) > (1.0/120) ) {
		update();
		accumulator = 0;
		}
		//	accumulator -= (1.0f); //by³o 1.0f / 59.0f
		//	if (accumulator < 0) accumulator = 0;
		display();
	}
	//if esc pomin to
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		apple_text = "CLOSING GAME";
		gltSetText(text[0], apple_text.c_str());
	}
	else {
		apple_text = "GAME OVER!";
		gltSetText(text[0], apple_text.c_str());
		for (int end_snake = 0; end_snake < 20; end_snake++) { //to ogolnie ma pokazac ze snake wyjechal poza plansze, piekne nie jest ale robi robote
			update();
		}
	}
	last = std::chrono::steady_clock::now();
	while (true) {
		current = std::chrono::steady_clock::now();
		processInput(window);
		display();
		difference = static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(current - last).count());
		if (difference > 1.0) {
			break;
		}
	}
	//TODO: przezucic do freeResources
	gltDeleteText(text[0]);
	gltDeleteText(text[1]);
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
	game_board = new Board(glm::ivec3( rand() % 20 + 10, 1, rand() % 20 + 10)); //TODO: parametry
	game_snake = new Snake(); //ssgame_board->generateSnakePosition());
	game_food = new Food(); // game_board->generateFoodPosition(game_snake->getCoords()));


	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3( -1.0f *(game_board->getDimensions().x/2), 3.0f, (-3.0f*game_board->getDimensions().z) ));
	view = glm::rotate(view, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	projection = glm::perspective(glm::radians(45.0f), (float(SCR_WIDTH) / float(SCR_HEIGHT)), 0.1f, 100.0f);


	//Drawing stuff
	gltInit(); //TODO: NEW!!!
}


void processInput(GLFWwindow* window)
{
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
	/*if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		if (mixer >= 0.9f) {
			mixer = 1.0f;
		}
		else if (mixer == 0.0f) {
			mixer = 0.1f;
		}
		else {
			mixer += (mixer * 0.1f);
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		if (mixer <= 0.1f) {
			mixer = 0.0f;
		}
		else {
			mixer -= (mixer * 0.1f);
		}
	}*/
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		//model = glm::rotate(model, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		//model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		view = glm::rotate(view, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		//model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.1f));
		//view = glm::rotate(view, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.1f));
		//view = glm::rotate(view, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		view = glm::translate(view, glm::vec3(0.0f, -0.1f, 0.0f));
		//view = glm::rotate(view, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		view = glm::translate(view, glm::vec3(0.0f, 0.1f, 0.0f));
		//view = glm::rotate(view, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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

void display()
{
	//TODO: usun
	//std::chrono::steady_clock::time_point last, current;
	//double difference = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Ustawienia t³a
	glClearColor(0.5f, 0.5f, 0.0f, 1.0f); //tutaj zmienna do zmiany koloru t³a + jakisVariable
	glClear(GL_COLOR_BUFFER_BIT);
	// Bindowanie tekstur
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture3);

	// W³¹czenie odpowiedniego shaderProgramu
	mainShader->use();
	mainShader->setFloat("texMix", mixer); //zmienne uniform shadera

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

void displayText(GLTtext* text, GLfloat x, GLfloat y, GLfloat scale) {

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.5f, 0.5f, 0.0f, 1.0f); 
	//glClear(GL_COLOR_BUFFER_BIT);
	
	mainShader->use();
	
	// Begin text drawing (this for instance calls glUseProgram)
	gltBeginDraw();
	// Draw any amount of text between begin and end
	gltColor(1.0f, 1.0f, 1.0f, 1.0f);
	gltDrawText2D(text, x, y, scale);
	// Finish drawing text
	gltEndDraw();
	
	//glfwSwapBuffers(window);
	//glfwPollEvents(); //TODO: whats up with this xd
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