#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <cstdio>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>

// globals for the sake of learning, will abstract away with classes when i'm not lazy
int gScreenWidth = 640;
int gScreenHeight = 480;
std::vector<float> positions = {
   -0.15f, -0.15f,  0.15f, // front bottom left
    0.15f, -0.15f,  0.15f, // front bottom right
   -0.15f,  0.15f,  0.15f, // front top left
    0.15f,  0.15f,  0.15f, // front top right
                      
   -0.15f, -0.15f, -0.15f, //  back bottom left
    0.15f, -0.15f, -0.15f, //  back bottom right
   -0.15f,  0.15f, -0.15f, //  back top left
    0.15f,  0.15f, -0.15f, //  back top right
};
std::vector<float> colors = {
    // first triangle
    1,   0,   0,
    0,   1,   0,
    0,   0,   1,
    // second triangle
    0,   1,   0,
    0,   0,   1,
    0,   0,   1
};
GLfloat g_uOffset = -1.0f;
GLfloat g_uScale = 1.0f;
int u_ModelMatrix;
int u_PerspectiveMatrix;
bool isUpPressed = false;
bool isDownPressed = false;
float degrees = 0.0f;

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   /* if (key == GLFW_KEY_UP && isUpPressed) {
        g_uOffset += 0.025f;
        std::cout << g_uOffset << std::endl;
    }

    if (key == GLFW_KEY_DOWN && isDownPressed) {
        g_uOffset -= 0.025f;
        std::cout << g_uOffset << std::endl;
    }*/
}   

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(gScreenWidth, gScreenHeight, "Hello cube!", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // set key input callback function
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // Problem: glewInit failed
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
    fprintf(stdout, "GL version: %s\n", glGetString(GL_VERSION));

    // vertex array object and buffer variables
    unsigned int vao;
    unsigned int bufferObj;
    unsigned int colorBufferObj;
    unsigned int indexBufferObj;

    // create vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* 3 step process of creating a buffer, generate->bind->assign data.
       Below, we create the buffer containing vertices. */
    glGenBuffers(1, &bufferObj);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // generate color buffer
    glGenBuffers(1, &colorBufferObj);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), colors.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // index buffer data
    std::vector<int> indices = {
        // front face
        2, 0, 1, 
        3, 2, 1,
        // left face
        1, 5, 3,
        7, 3, 5,
        // right face
        5, 7, 4,
        4, 6, 7,
        // back face
        4, 6, 2,
        2, 0, 4
    };

    // generate index buffer
    glGenBuffers(1, &indexBufferObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // GLSL vertex and fragment shader source code stored in strings
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec3 vertexColors;"

        "uniform mat4 u_ModelMatrix;"
        "uniform mat4 u_Perspective;"

        "out vec3 v_VertexColors;"

        "void main()\n"
        "{\n"
        "   vec4 newPosition = u_Perspective * u_ModelMatrix * vec4(position, 1.0f);"
                                                                            // dont forget w!
        "   gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);\n"
        "   v_VertexColors = vertexColors\n;"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "in vec3 v_VertexColors;\n"
        "out vec4 fragColor;"

        "void main()\n"
        "{\n"
        "   fragColor = vec4(v_VertexColors, 1.0f);\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    // create model matrix
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, g_uOffset));
    // get location of model matrix
    int modelMatrixLocation = glGetUniformLocation(shader, "u_ModelMatrix");

    // error checks
    if (modelMatrixLocation >= 0) {
        std::cout << modelMatrixLocation << std::endl;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
    }
    else {
        std::cout << "Could not find location of u_ModelMatrix." << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Projection matrix (perspective)
    * [ 1/(aspect*tan(fov/2)),      0,                    0,                            0             ]
    * [          0,            1/tan(fov/2),              0,                            0             ]                               
    * [          0,                 0,         -((far+near)/(far-near))    -(*2*far*near)/(far-near)) ]
    * [          0,                 0,                   -1,                            0             ]
    */
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), 
                                            (float) (gScreenWidth / gScreenHeight), 
                                            0.1f,
                                            10.0f);

    // get location of perspective matrix
    int projectionLocation = glGetUniformLocation(shader, "u_Perspective");

    // error checks
    if (projectionLocation >= 0) {
        std::cout << projectionLocation << std::endl;
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &perspective[0][0]);
    }
    else {
        std::cout << "Could not find location of u_Perspective." << std::endl;
        exit(EXIT_FAILURE);
    }

    // unbind current buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // setup fps calculation variables
    double lastTime = glfwGetTime();
    double currentTime = 0;
    int frames = 0;

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        currentTime = glfwGetTime();
        frames++;

        if (currentTime - lastTime >= 1.0f) {
            std::cout << frames << std::endl;
            frames = 0;
            lastTime = currentTime;
        }

        // update perspective matrix and uniform variable
        perspective = glm::perspective(glm::radians(45.0f),
            (float)gScreenWidth / (float)gScreenHeight,
            0.1f,
            1.0f);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &perspective[0][0]);

        // update model matrix and uniform variable
        modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, g_uOffset));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(g_uScale, g_uScale, g_uScale));
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        if (degrees >= 360) degrees = 0;
        degrees++;

        // draw call
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            g_uOffset += 0.025f;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            g_uOffset -= 0.025f;
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            g_uScale += 0.025f;
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            g_uScale -= 0.025f;
        }
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}