#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Application.hpp>
#include <iostream>

Application::Application() {
    // Initialize the library
    if (!glfwInit()) {
        std::cout << "init fail" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create a windowed mode window and its OpenGL context
    m_Window = glfwCreateWindow(m_ScreenWidth, m_ScreenHeight, "Hello cube!", NULL, NULL);
    if (!m_Window)
    {
        std::cout << "window fail";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make the window's context current
    glfwMakeContextCurrent(m_Window);

    m_Err = glewInit();
    if (GLEW_OK != m_Err)
    {
        // Problem: glewInit failed
        fprintf(stderr, "Error: %s\n", glewGetErrorString(m_Err));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
    fprintf(stdout, "GL version: %s\n", glGetString(GL_VERSION));
}