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
void CalculateDeltaTime();
unsigned int GeneratePlane(const char* heightmap, unsigned char*& data, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID);
float CalculateCameraSpeed(GLFWwindow* window, float& cameraPower);
void processInput(GLFWwindow* window, glm::vec3& cameraFront, glm::vec3& cameraPos, glm::vec3& cameraUp);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void CreateGeometry(GLuint& vao, GLuint& EBO, int& size, int& numbIndices);
void createShaders();
void createProgram(GLuint& programID, const char* vertex, const char* fragment);
GLuint loadTexture(const char* path);
void renderSkyBox();
void renderTerrain();


// Util
void loadFile(const char* filename, char*& output);

//programIDs
GLuint simpleProgram;
GLuint skyProgram;
GLuint terrainProgram;

const int WIDTH = 1280;
const int HEIGHT = 720;

// time 
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// camera 
glm::vec3 cameraPosition = glm::vec3(0.0f, 200.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float normalPower = 25.0f;
float fastPower = 50.0f;

// mouse Input;
float lastX = WIDTH / 2;
float lastY = HEIGHT / 2;
float yaw = -90.0f;
float pitch = 0;
bool firstMouse = true;

// World Data
glm::vec3 lightDirection = glm::normalize(glm::vec3(0, 0.5f, 0.5f));
GLuint boxVAO, boxEBO;
int boxSize, boxIndexCount;

glm::mat4 view;
glm::mat4 projection;

// Terrain data
GLuint terrainVAO, terrainIndexCount, heightmapID, heightNormalID;
unsigned char* heightmapTexture;

int main()
{

	//init
	GLFWwindow* window;
	int res = init(window);
	if (res != 0) return res;

	glfwSetCursorPosCallback(window, mouse_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	createShaders();
	CreateGeometry(boxVAO, boxEBO, boxSize, boxIndexCount);

	terrainVAO = GeneratePlane("textures/Heightmap.png", heightmapTexture, GL_RGBA, 4, 250.0f, 5.0f, terrainIndexCount, heightmapID);
	heightNormalID = loadTexture("textures/heightnormal.png");
	GLuint boxTex = loadTexture("textures/container2.png");
	GLuint boxNormal = loadTexture("textures/container2_normal.png");



	// Create Viewport
	glViewport(0, 0, WIDTH, HEIGHT);



	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 cameraDirection = glm::normalize(cameraPosition - cameraTarget);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

	//matrices!
	projection = glm::perspective(glm::radians(45.0f), WIDTH / (float)HEIGHT, 0.1f, 5000.0f);


	// Game render loop
	while (!glfwWindowShouldClose(window)) {
		CalculateDeltaTime();

		// input handling (TODO)
		processInput(window, cameraFront, cameraPosition, cameraUp);
		view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

		//rendering
		glClearColor(0.2, 0.3, 0.3, 1.0); //beautiful green
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		renderSkyBox();
		renderTerrain();


		glUseProgram(simpleProgram);

		glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "world"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(simpleProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glUniform3fv(glGetUniformLocation(simpleProgram, "lightposition"), 1, glm::value_ptr(lightDirection));
		glUniform3fv(glGetUniformLocation(simpleProgram, "cameraposition"), 1, glm::value_ptr(cameraPosition));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, boxTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE1, boxNormal);


		glBindVertexArray(boxVAO);
		//glDrawArrays(GL_TRIANGLES, 0, triangleSize);
		glDrawElements(GL_TRIANGLES, boxIndexCount, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window);

		//evemts pollen
		glfwPollEvents();

	}
	glDisable(GL_CULL_FACE);

	// terminate
	glfwTerminate();

	return 0;
}
void renderSkyBox() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH);


	glUseProgram(skyProgram);
	//matrices!
	glm::mat4 world = glm::mat4(1.0f);
	world = glm::translate(world, cameraPosition);
	world = glm::scale(world, glm::vec3(10, 10, 10));

	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(skyProgram, "lightDirection"), 1, glm::value_ptr(lightDirection));
	glUniform3fv(glGetUniformLocation(skyProgram, "cameraPosition"), 1, glm::value_ptr(cameraPosition));

	//rendering
	glBindVertexArray(boxVAO);
	glDrawElements(GL_TRIANGLES, boxIndexCount, GL_UNSIGNED_INT, 0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH);



}

void renderTerrain() {
	glEnable(GL_DEPTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glUseProgram(terrainProgram);
	//matrices!
	glm::mat4 world = glm::mat4(1.0f);
	//world = glm::translate(world, cameraPosition);
	//world = glm::scale(world, glm::vec3(10, 10, 10));

	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(terrainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3fv(glGetUniformLocation(terrainProgram, "lightDirection"), 1, glm::value_ptr(lightDirection));
	glUniform3fv(glGetUniformLocation(terrainProgram, "cameraPosition"), 1, glm::value_ptr(cameraPosition));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightmapID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightNormalID);

	//rendering
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);
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

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;
	//pitch = glm::clamp(pitch - ypos, -90.0f, 90.0f);

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window, glm::vec3& cameraFront, glm::vec3& cameraPos, glm::vec3& cameraUp) {
	// escape program
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	//get cameraSpeed
	float cameraPower = 2.5f;
	float cameraSpeed = CalculateCameraSpeed(window, cameraPower);

	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// check move Input program
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += worldUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= worldUp * cameraSpeed;
}

float CalculateCameraSpeed(GLFWwindow* window, float& cameraPower) {
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		cameraPower = fastPower;
	}
	else {
		cameraPower = normalPower;
	}
	float cameraSpeed = cameraPower * deltaTime;
	return cameraSpeed;
}

void CreateGeometry(GLuint& vao, GLuint& EBO, int& size, int& numbIndices) {

	float vertices[] = {
		// positions            //colors            // tex coords   // normals          //tangents      //bitangents
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,     -1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,      0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		-0.5f, 0.5f, -.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,
		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,     1.f, 0.f, 0.f,  0.f, -1.f, 0.f,

		-0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,
		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       -1.f, 0.f, 0.f,     0.f, 1.f, 0.f,  0.f, 0.f, 1.f,

		-0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,
		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,     1.f, 0.f, 0.f,  0.f, 1.f, 0.f,

		0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       1.f, 0.f, 0.f,     0.f, -1.f, 0.f,  0.f, 0.f, 1.f,

		0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 1.f, 0.f,     1.f, 0.f, 0.f,  0.f, 0.f, 1.f
	};

	unsigned int indices[] = {  // note that we start from 0!
		// DOWN
		0, 1, 2,   // first triangle
		0, 2, 3,    // second triangle
		// BACK
		14, 6, 7,   // first triangle
		14, 7, 15,    // second triangle
		// RIGHT
		20, 4, 5,   // first triangle
		20, 5, 21,    // second triangle
		// LEFT
		16, 8, 9,   // first triangle
		16, 9, 17,    // second triangle
		// FRONT
		18, 10, 11,   // first triangle
		18, 11, 19,    // second triangle
		// UP
		22, 12, 13,   // first triangle
		22, 13, 23,    // second triangle
	};


	int stride = (3 + 3 + 2 + 3 + 3 + 3) * sizeof(float);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);


	// set layout of vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, stride, (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_TRUE, stride, (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(5, 3, GL_FLOAT, GL_TRUE, stride, (void*)(14 * sizeof(float)));
	glEnableVertexAttribArray(5);
}

void createShaders() {
	createProgram(simpleProgram, "shaders/simpleVertex.shader", "shaders/simpleFragment.shader");
	//set texture channels
	glUseProgram(simpleProgram);
	glUniform1i(glGetUniformLocation(simpleProgram, "mainTex"), 0);
	glUniform1i(glGetUniformLocation(simpleProgram, "normalTex"), 1);


	createProgram(skyProgram, "shaders/SkyVertex.shader", "shaders/SkyFragment.shader");

	createProgram(terrainProgram, "shaders/TerrainVertex.shader", "shaders/TerrainFragment.shader");

	glUseProgram(terrainProgram);
	glUniform1i(glGetUniformLocation(terrainProgram, "mainTex"), 0);
	glUniform1i(glGetUniformLocation(terrainProgram, "normalTex"), 1);
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

	if (data) {
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

void CalculateDeltaTime() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

unsigned int GeneratePlane(const char* heightmap, unsigned char*& data, GLenum format, int comp, float hScale, float xzScale, unsigned int& indexCount, unsigned int& heightmapID) {
	int width, height, channels;
	data = nullptr;
	if (heightmap != nullptr) {
		data = stbi_load(heightmap, &width, &height, &channels, comp);
		if (data) {
			glGenTextures(1, &heightmapID);
			glBindTexture(GL_TEXTURE_2D, heightmapID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	int stride = 8;
	float* vertices = new float[(width * height) * stride];
	unsigned int* indices = new unsigned int[(width - 1) * (height - 1) * 6];

	int index = 0;
	for (int i = 0; i < (width * height); i++) {
		// TODO: calculate x/z values
		int x = i % width;
		int z = i / width;

		float texHeigh = (float)data[i * comp];

		// TODO: set position
		vertices[index++] = x * xzScale;
		vertices[index++] = (texHeigh / 255.0f) * hScale;
		vertices[index++] = z * xzScale;

		// TODO: set normal
		vertices[index++] = 0;
		vertices[index++] = 1;
		vertices[index++] = 0;

		// TODO: set uv
		vertices[index++] = x / (float)width;
		vertices[index++] = z / (float)height;

	}

	// OPTIONAL TODO: Calculate normal
	// TODO: Set normal

	index = 0;
	for (int i = 0; i < (width - 1) * (height - 1); i++) {
		int x = i % (width - 1);
		int z = i / (width - 1);

		int vertex = z * width + x;

		indices[index++] = vertex;
		indices[index++] = vertex + width;
		indices[index++] = vertex + width + 1;

		indices[index++] = vertex;
		indices[index++] = vertex + width + 1;
		indices[index++] = vertex + 1;

	}

	unsigned int vertSize = (width * height) * stride * sizeof(float);
	indexCount = ((width - 1) * (height - 1) * 6);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// vertex information!
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, 0);
	glEnableVertexAttribArray(0);
	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	// uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	delete[] vertices;
	delete[] indices;

	//stbi_image_free(data);

	return VAO;
}