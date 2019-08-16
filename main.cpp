//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // gets rid of the command line

#include <glad/glad.h>
#include <GLFW\glfw3.h>

#include <iostream>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "enums.h"
#include "board.h"
#include "snake.h"
#include "food.h"
#include "shader_loader.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Game Globals
directions snake_dir;
options game_option;
bool game_over;
Board* g_board;
Snake* g_snake;
Food* g_food;

//TODO: opengl stuff
void monitor_callback(GLFWmonitor* monitor, int event);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 1000; // get from windows?
unsigned int SCR_HEIGHT = 1000;

GLFWwindow* window;


//Shaders

Shader *mainShader, *secondaryShader;


//===================================test
GLuint VBOs[8], VAOs[8], EBO, vertexShader, fragmentShader, shaderProgram, shaderProgram2,
texture1, texture2;
// an optional way of delcaring opengl objs -> GLuint shaderSuperProgram;

// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

glm::mat4 model;

glm::mat4 view;

glm::mat4 projection;

float mixer = 0.5f;

glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};

float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

unsigned int indices[] = {
   0, 1, 3, // first triangle
   1, 2, 3  // second triangle
};

/*
glm::mat4 trans = glm::mat4(1.0f);
trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
vec = trans * vec;
std::cout << vec.x << vec.y << vec.z << std::endl;*/


//====================================test

//void processInput(GLFWwindow *window);
void update(void);// elapsed);
void render(void);
void handle_options(void);
void initialize(void);


int main()
{
	initialize();
	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);

	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(45.0f), (float(SCR_WIDTH) / float(SCR_HEIGHT)), 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST);

	//TODO: glUseProgram(shaderProgram); // w petli

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window) && (!game_over))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// input
		// -----
		processInput(window);

		// render
		// ------


		float greenValue = sin(glfwGetTime()) / 2.0f + 0.5f;

		glClearColor(1.0f - greenValue, 0.0f + greenValue, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, 0.1f, 100.0f);

		mainShader->use();
		mainShader->setFloat("texMix", mixer);


		//local space -> world space -> view space -> clip space -> screen space

		glBindVertexArray(VAOs[0]);

		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			if (i % 2 != 0) {
				model = glm::rotate(model, glm::radians((float)(5*glfwGetTime())), glm::vec3(1.0f, 0.0f, 0.0f));
			}
			mainShader->setMat4("model", model);
			mainShader->setMat4("view", view);
			mainShader->setMat4("projection", projection);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//glDrawArrays(GL_TRIANGLES, 0, 1*3);

		glBindVertexArray(0); // no need to unbind it every time 

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// najpierw genertuj g_board potem -> g_snake -> g_food


	/* sort of a game loop
	double previous = getCurrentTime();
	double lag = 0.0;
	while (true)
	{
	 double current = getCurrentTime();
	 double elapsed = current - previous;
	 previous = current;
	 lag += elapsed;

	 processInput();

	while (lag >= MS_PER_UPDATE)
	{
	  update();
	 lag -= MS_PER_UPDATE;
	}

	render();render(lag / MS_PER_UPDATE);
}

	*/


	glDeleteVertexArrays(1, VAOs);
	glDeleteBuffers(1, VBOs);//FIXME:
	glDeleteBuffers(1, &EBO);
	//glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}


void initialize()
{
	game_over = false;
	snake_dir = left;
	g_board = new Board(); //parametry
	g_snake = new Snake();
	g_food = new Food();

	// glfw: initialize and configure
	// ------------------------------
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
	const GLFWvidmode* primary_mode = glfwGetVideoMode(primary);
	const GLFWvidmode* secondary_mode = glfwGetVideoMode(secondary);
	if (monitor_count < 2) {
		SCR_WIDTH = primary_mode->width;
		SCR_HEIGHT = primary_mode->height;
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Snake-3D", primary, NULL); //monitors[0]
	}
	else {
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

	

	//all the vbos and stuff
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
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
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

	glGenVertexArrays(1, VAOs);
	glGenBuffers(1, VBOs); //FIXME: & uzywam tylko jednego typu na razie
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); //unbinding 
	
	
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
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
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		//model = glm::rotate(model, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		//model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		view = glm::rotate(view, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
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
	/*
	if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (snake_dir != downwards)) {
		snake_dir = upwards;
	}
	else if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (snake_dir != upwards)) {
		snake_dir = downwards;
	}
	else if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && (snake_dir != left)) {
		snake_dir = right;
	}
	else if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && (snake_dir != right)) {
		snake_dir = left;
	}
	//else
	if ((glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS ) )
	{
		game_option = rotate_left;//rotate camera left
	}
	else if ((glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS))
	{
		game_option = rotate_right;//rotate camera right
	}
	else if ((glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS))
	{
		game_option = show_menu;
	}
	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}*/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void update()//elapsed);
{
	game_over = g_snake->Move(snake_dir, g_board, g_food);
	//if g_food eaten g_food = new Food();
	//menu tutaj?
	handle_options();
}

void render()
{
	// thats gon be a pain XD
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