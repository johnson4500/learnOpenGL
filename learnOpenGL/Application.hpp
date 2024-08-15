#ifndef APP_HPP
#define APP_HPP
#include <GLFW/glfw3.h>

struct Application {
	int m_ScreenWidth = 640;
	int m_ScreenHeight = 480;
	GLFWwindow* m_Window;
	GLenum m_Err;
	unsigned int m_ShaderProgram;

	// setup fps calculation variables
	double m_LastTime;
	double m_CurrentTime = 0;
	int m_Frames = 0;
};


#endif