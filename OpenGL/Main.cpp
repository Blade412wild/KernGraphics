#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
    glfwInit();

    // Tell GLFW which profile & openGL version we're using
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Create Window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Unity", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to Create window" << std::endl;
        return -1;
    }

    // Set content
    glfwMakeContextCurrent(window);

    // GLad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to load Glad" << std::endl;
        glfwTerminate();
        return -2;
    }


    // Create Viewport
    glViewport(0, 0, 1280, 720);

    // Game render loop
    while (!glfwWindowShouldClose(window)) {
        // input handling (TODO)

        //rendering
        // background color set & render!
        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        //evemts pollen
        glfwPollEvents();
    }

    // terminate
    glfwTerminate();

    std::cout << "Hello World" << std::endl;
    return 0;
}