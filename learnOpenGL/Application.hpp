#ifndef APP_HPP
#define APP_HPP
#include <GLFW/glfw3.h>

class Application {
	public:
		int m_ScreenWidth = 640;
		int m_ScreenHeight = 480;
		GLFWwindow* m_Window;
		GLenum m_Err;
		unsigned int m_ShaderProgram;

		Application();
};


#endif