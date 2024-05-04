#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// forward Declarations
int init(GLFWwindow*& window);
void CreateTriangle(GLuint& vao, GLuint& EBO, int& size, int& numbIndices);
void createShaders();
void createProgram(GLuint& programID, const char* vertex, const char* fragment);


// Util
void loadFile(const char* filename, char*& output);

//programIDs
GLuint simpleProgram;

int main()
{
	//init
	GLFWwindow* window;
	int res = init(window);
	if (res != 0) return res;

	GLuint triangleVAO, triangleEBO;
	int triangleSize, triangleIndexCount;

	CreateTriangle(triangleVAO, triangleEBO, triangleSize, triangleIndexCount);
	createShaders();

	// Create Viewport
	glViewport(0, 0, 1280, 720);

	// Game render loop
	while (!glfwWindowShouldClose(window)) {
		// input handling (TODO)

		//rendering
		glClearColor(0.2, 0.3, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(simpleProgram);

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
	window = glfwCreateWindow(1280, 720, "Unity", nullptr, nullptr);

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

void CreateTriangle(GLuint& vao, GLuint& EBO, int& size, int& numbIndices) {
	float vertices[] = {
		// position				//color
		-0.5, -0.5, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
		0.5, -0.5, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f,
		-0.5, 0.5, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
		0.5, 0.5, 0.0f,			1.0f, 1.0f, 1.0f, 1.0f,
	};

	int indices[] = {
		0, 1, 2,
		2, 1, 3
	};


	int stride = (3 + 4) * sizeof(float);
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

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);
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