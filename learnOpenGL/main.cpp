#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Application.hpp>
#include <Mesh3D.hpp>
#include <cstdio>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>

Application App = Application();
Mesh3D mesh1 = Mesh3D();
bool isUpPressed = false;
bool isDownPressed = false;

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
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

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
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

//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
// 
//}   

void initializeProgram(Application *App) {
    // Initialize the library
    if (!glfwInit())
        exit(EXIT_FAILURE);

    // Create a windowed mode window and its OpenGL context
    App->m_Window = glfwCreateWindow(App->m_ScreenWidth, App->m_ScreenHeight, "Hello cube!", NULL, NULL);
    if (!App->m_Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make the window's context current
    glfwMakeContextCurrent(App->m_Window);

    App->m_Err = glewInit();
    if (GLEW_OK != App->m_Err)
    {
        // Problem: glewInit failed
        fprintf(stderr, "Error: %s\n", glewGetErrorString(App->m_Err));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
    fprintf(stdout, "GL version: %s\n", glGetString(GL_VERSION));
}

void vertexSpecification(Mesh3D *mesh) {

    // position buffer data
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

    // color buffer data
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
        2, 0, 4,
        // top face
        3, 7, 6,
        3, 6, 2,
        // bottom face
        1, 5, 4,
        1, 4, 0
    };
    // create vertex array object
    glGenVertexArrays(1, &(mesh->m_VAO));
    glBindVertexArray(mesh->m_VAO);

    /* 3 step process of creating a buffer, generate->bind->assign data.
       Below, we create the buffer containing vertices. */
    glGenBuffers(1, &(mesh->m_BufferObj));
    glBindBuffer(GL_ARRAY_BUFFER, mesh->m_BufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // generate color buffer
    glGenBuffers(1, &(mesh->m_ColorBufferObj));
    glBindBuffer(GL_ARRAY_BUFFER, mesh->m_ColorBufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), colors.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // generate index buffer
    glGenBuffers(1, &(mesh->m_IndexBufferObj));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_IndexBufferObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    //// unbind current buffer
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    // Disable any open attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void createGraphicsPipeline(Application *App) {
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

    App->m_ShaderProgram = CreateShader(vertexShader, fragmentShader);
    //glUseProgram(App->m_ShaderProgram);
}

void preDraw(Mesh3D *mesh, Application *app) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(app->m_ShaderProgram);

    // create model matrix and apply transformations
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, mesh->m_uOffset));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(mesh->m_uRotateDegrees), glm::vec3(1.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(mesh->m_uScale, mesh->m_uScale, mesh->m_uScale));

    // get location of model matrix
    int modelMatrixLocation = glGetUniformLocation(app->m_ShaderProgram, "u_ModelMatrix");

    // error checks
    if (modelMatrixLocation >= 0) {
        //std::cout << modelMatrixLocation << std::endl;
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
        (float)app->m_ScreenWidth / (float)app->m_ScreenHeight,
        0.1f,
        10.0f);

    // get location of perspective matrix
    int projectionLocation = glGetUniformLocation(app->m_ShaderProgram, "u_Perspective");

    // error checks
    if (projectionLocation >= 0) {
        //std::cout << projectionLocation << std::endl;
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &perspective[0][0]);
    } else {
        std::cout << "Could not find location of u_Perspective." << std::endl;
        exit(EXIT_FAILURE);
    }

    // increase cube rotation with each frame
    if (mesh1.m_uRotateDegrees >= 360) mesh1.m_uRotateDegrees = 0;
    mesh1.m_uRotateDegrees += 0.05f;
}

void drawMesh(Mesh3D *mesh, unsigned int pipeline) {
    // per mesh, choose pipeline to be used
    glUseProgram(pipeline);
    
    // enable attributes
    glBindVertexArray(mesh->m_VAO);

    // render data
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // stop using current pipeline
    glUseProgram(0);
}

void keyInput(Application *App) {
    if (glfwGetKey(App->m_Window, GLFW_KEY_UP) == GLFW_PRESS) {
        mesh1.m_uOffset += 0.025f;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        mesh1.m_uOffset -= 0.025f;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_E) == GLFW_PRESS) {
        mesh1.m_uScale += 0.025f;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_Q) == GLFW_PRESS) {
        mesh1.m_uScale -= 0.025f;
    }
}

void mainLoop(Application *App) {
    // loop until the user closes the window
    while (!glfwWindowShouldClose(App->m_Window)) {
        App->m_LastTime = glfwGetTime();
        // calculate frames per second
        /*App->m_LastTime = App->m_CurrentTime;*/
        App->m_CurrentTime = glfwGetTime();
        /*float dt = App->m_CurrentTime - App->m_LastTime;
        std::cout << dt << std::endl;*/
        App->m_Frames++;
        if (App->m_CurrentTime - App->m_LastTime >= 1.0f) {
            std::cout << App->m_Frames << std::endl;
            App->m_Frames = 0;
            App->m_LastTime = App->m_CurrentTime;
        }

        // input
        keyInput(App);
        
        // predraw
        preDraw(&mesh1, App);

        // draw call
        drawMesh(&mesh1, App->m_ShaderProgram);

        // Swap front and back buffers
        glfwSwapBuffers(App->m_Window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

int main(void) {
    initializeProgram(&App);
    
    vertexSpecification(&mesh1);

    createGraphicsPipeline(&App);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    mainLoop(&App);

    glDeleteProgram(App.m_ShaderProgram);
    glfwTerminate();
    return 0;
}