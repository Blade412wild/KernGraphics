#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// forward Declarations
int init(GLFWwindow*& window);
void CreateGeometry(GLuint& vao, GLuint& EBO, int& size, int& numbIndices);
void createShaders();
void createProgram(GLuint& programID, const char* vertex, const char* fragment);
GLuint loadTexture(const char* path);


// Util
void loadFile(const char* filename, char*& output);

//programIDs
GLuint simpleProgram;

const int WIDTH = 1280;
const int HEIGHT = 720;

int main()
{
	//init
	GLFWwindow* window;
	int res = init(window);
	if (res != 0) return res;

	createShaders();
	GLuint triangleVAO, triangleEBO;
	int triangleSize, triangleIndexCount;

	CreateGeometry(triangleVAO, triangleEBO, triangleSize, triangleIndexCount);

	GLuint boxTex = loadTexture("texture/container2.png");
	// Create Viewport
	glViewport(0, 0, WIDTH, HEIGHT);

	//matrices!
	glm::mat4 world = glm::mat4(1.0f);
	world = glm::rotate(world, glm::radians(45.0f), glm::vec3(0, 1, 0));
	world = glm::scale(world, glm::vec3(1, 1, 1));
	world = glm::translate(world, glm::vec3(0, 0, 0));

	glm::mat4 view = glm::lookAt(glm::vec3(0, 2.5, -5.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 projection = glm::perspective(45.0f, WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		// Game render loop
		while (!glfwWindowShouldClose(window)) {
			// input handling (TODO)

			//rendering
			glClearColor(0.2, 0.3, 0.3, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(simpleProgram);

			glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
			glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(triangleVAO);
			glDrawArrays(GL_TRIANGLES, 0, triangleSize);
			glDrawElements(GL_TRIANGLES, triangleIndexCount, GL_UNSIGNED_INT, 0);

			glfwSwapBuffers(window);

			//evemts pollen
			glfwPollEvents();
		}

	// terminate
	glfwTerminate();

	std::cout << "Hello World" << std::endl;
	return 0;
}

int init(GLFWwindow*& window) {
	glfwInit();

	// Tell GLFW which profile & openGL version we're using
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Create Window
	window = glfwCreateWindow(WIDTH, HEIGHT, "Unity", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to Create window" << std::endl;
		return -1;
	}

	// Set content
	glfwMakeContextCurrent(window);

	// load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load Glad" << std::endl;
		glfwTerminate();
		return -2;
	}
	return 0;
}

void CreateGeometry(GLuint& vao, GLuint& EBO, int& size, int& numbIndices) {
	//float vertices[] = {
	//	// position				//color
	//	-0.5, -0.5, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
	//	0.5, -0.5, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f,
	//	-0.5, 0.5, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	//	0.5, 0.5, 0.0f,			1.0f, 1.0f, 1.0f, 1.0f,
	//};

	float vertices[] = {
		// position				//color					//Tex coords		//normals
		0.5, -0.5, -0.5f,		1.0f, 1.0f, 1.0f,		1.0f, 0.0f,			0.0, -1.0f, 0.0f,
		0.5, -0.5, 0.5f,		1.0f, 1.0f, 1.0f,		1.0f, 1.0f,			0.0, -1.0f, 0.0f,
		-0.5, -0.5, 0.5f,		1.0f, 1.0f, 1.0f,		0.0f, 1.0f,			0.0, -1.0f, 0.0f,
		-0.5, -0.5, -0.5f,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,			0.0, -1.0f, 0.0f,

		// 1
		0.5, 0.5, -0.5f,		1.0f, 1.0f, 1.0f,		2.0f, 0.0f,			1.0, 0.0f, 0.0f,
		0.5, 0.5, 0.5f,			1.0f, 1.0f, 1.0f,		2.0f, 1.0f,			1.0, 0.0f, 0.0f,

		// 2
		0.5, 0.5, 0.5f,			1.0f, 1.0f, 1.0f,		2.0f, 0.0f,			0.0, 0.0f, 1.0f,
		-0.5, 0.5, 0.5f,		1.0f, 1.0f, 1.0f,		2.0f, 1.0f,			0.0, 0.0f, 1.0f,

		// 3
		-0.5, 0.5, 0.5f,		1.0f, 1.0f, 1.0f,		-1.0f, 1.0f,		-1.0, 0.0f, 0.0f,
		-0.5, 0.5, -0.5f,		1.0f, 1.0f, 1.0f,		-1.0f, 0.0f,		-1.0, 0.0f, 0.0f,

		// 4
		-0.5, 0.5, 0.5f,		1.0f, 1.0f, 1.0f,		0.0f, -1.0f,		0.0, 0.0f, -1.0f,
		0.5, 0.5, 0.5f,			1.0f, 1.0f, 1.0f,		1.0f, -1.0f,		0.0, 0.0f, -1.0f,

		// 5
		-0.5, 0.5, -0.5f,		1.0f, 1.0f, 1.0f,		3.0f, 0.0f,			0.0, 1.0f, 0.0f,
		-0.5, 0.5, 0.5f,		1.0f, 1.0f, 1.0f,		3.0f, 1.0f,			0.0, 1.0f, 0.0f,

		// 6
		0.5, -0.5, 0.5f,		1.0f, 1.0f, 1.0f,		0.0f, 1.0f,			0.0, 0.0f, 1.0f,
		-0.5, -0.5, 0.5f,		1.0f, 1.0f, 1.0f,		1.0f, 1.0f,			0.0, 0.0f, 1.0f,

		// 7
		-0.5, -0.5, 0.5f,		1.0f, 1.0f, 1.0f,		0.0f, 1.0f,			-1.0, 0.0f, 0.0f,
		-0.5, -0.5, -0.5f,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,			-1.0, 0.0f, 0.0f,

		// 8
		-0.5, -0.5, -0.5f,		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,			0.0, 0.0f, -1.0f,
		0.5, -0.5, -0.5f,		1.0f, 1.0f, 1.0f,		1.0f, 0.0f,			0.0, 0.0f, -1.0f,

		// 9
		0.5, -0.5, -0.5f,		1.0f, 1.0f, 1.0f,		1.0f, 0.0f,			1.0, 0.0f, 0.0f,
		0.5, -0.5, 0.5f,		1.0f, 1.0f, 1.0f,		1.0f, 1.0f,			1.0, 0.0f, 0.0f,

		// 10
		0.5, 0.5, -0.5f,		1.0f, 1.0f, 1.0f,		2.0f, 0.0f,			0.0, 1.0f, 0.0f,
		0.5, 0.5, 0.5f,			1.0f, 1.0f, 1.0f,		2.0f, 1.0f,			0.0, 1.0f, 0.0f,

	};

	//int indices[] = {
	//	0, 1, 2,
	//	2, 1, 3
	//};

	int indices[] = {
		// down
		0, 1, 2,
		0, 2, 3,

		// back
		14, 6, 7,
		14, 7, 15,

		//right
		20, 4, 5,
		20, 5, 21,

		//left
		16, 8, 9,
		16, 9, 17,

		//front
		18, 10, 11,
		18, 11, 19,

		//up
		22, 12, 03,
		22, 13, 23,

	};


	int stride = (3 + 3 + 2 + 3) * sizeof(float);
	size = sizeof(vertices) / stride;
	numbIndices = sizeof(indices) / sizeof(int);

	// create te VAO
	glGenVertexArrays(1, &vao);
	//Bind it to create its configuration
	glBindVertexArray(vao);

	// create buffer, bind it & assign vertices to it
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// set layout of vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void createShaders() {

	createProgram(simpleProgram, "shaders/simpleVertex.shader", "shaders/simpleFragment.shader");
}

void createProgram(GLuint& programID, const char* vertex, const char* fragment) {
	//create a GL Program with a Vertex & Fragment Shader

	char* vertexScr;
	char* fragmentScr;
	loadFile(vertex, vertexScr);
	loadFile(fragment, fragmentScr);

	GLuint vertexShaderID, fragmentShaderID;

	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexScr, nullptr);
	glCompileShader(vertexShaderID);

	int succes;
	char infoLog[512];
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &succes);
	if (!succes) {
		glGetShaderInfoLog(vertexShaderID, 512, nullptr, infoLog);
		std::cout << "ERROR COMPILING VERTEX SHADER \n " << infoLog << std::endl;
	}

	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentScr, nullptr);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &succes);
	if (!succes) {
		glGetShaderInfoLog(fragmentShaderID, 512, nullptr, infoLog);
		std::cout << "ERROR COMPILING Fragment SHADER \n " << infoLog << std::endl;
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &succes);
	if (!succes) {
		glGetProgramInfoLog(programID, 512, nullptr, infoLog);
		std::cout << "ERROR LINKING PRORGAM \n " << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	delete vertexScr;
	delete fragmentScr;

}

void loadFile(const char* filename, char*& output) {

	//open file stream
	std::ifstream file(filename, std::ios::binary);

	//if the file was succesfully opened
	if (file.is_open()) {
		//get lenght of file
		file.seekg(0, file.end);
		int lenght = file.tellg();
		file.seekg(0, file.beg);

		//allocate memory for the char pointer
		output = new char[lenght + 1];

		// read datat as a block
		file.read(output, lenght);

		//add null terminator rto end of char pointer
		output[lenght] = '\0';

		// close the file
		file.close();
	}
	else {
		// if the file failed to open, set the chart pointer to NULL
		output = NULL;
	}
}

GLuint loadTexture(const char* path) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, numChannel;

	unsigned char* data = stbi_load(path, &width, &height, &numChannel, 0);

	if (!data) {
		if (numChannel == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (numChannel == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR LOADING TEXTURE: " << path << std::endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}