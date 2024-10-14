#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
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
Mesh3D mesh2 = Mesh3D();
bool isUpPressed = false;
bool isDownPressed = false;
unsigned int texture;
int widthImg, heightImg, numColCh;

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

void initializeProgram(Application* App) {
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

void vertexSpecification(Mesh3D* mesh) {
    // position buffer data
    std::vector<float> positions = {
        // Top
         -1.0, 1.0, -1.0,   0, 0,
         -1.0, 1.0, 1.0,    0, 1,
         1.0, 1.0, 1.0,     1, 1,
         1.0, 1.0, -1.0,    1, 0,

         // Left
         -1.0, 1.0, 1.0,    0, 0,
         -1.0, -1.0, 1.0,   1, 0,
         -1.0, -1.0, -1.0,  1, 1,
         -1.0, 1.0, -1.0,   0, 1,

         // Right
         1.0, 1.0, 1.0,    1, 1,
         1.0, -1.0, 1.0,   0, 1,
         1.0, -1.0, -1.0,  0, 0,
         1.0, 1.0, -1.0,   1, 0,

         // Front
         1.0, 1.0, 1.0,    1, 1,
         1.0, -1.0, 1.0,    1, 0,
         -1.0, -1.0, 1.0,    0, 0,
         -1.0, 1.0, 1.0,    0, 1,

         // Back
         1.0, 1.0, -1.0,    0, 0,
         1.0, -1.0, -1.0,    0, 1,
         -1.0, -1.0, -1.0,    1, 1,
         -1.0, 1.0, -1.0,    1, 0,

         // Bottom
         -1.0, -1.0, -1.0,   1, 1,
         -1.0, -1.0, 1.0,    1, 0,
         1.0, -1.0, 1.0,     0, 0,
         1.0, -1.0, -1.0,    0, 1,
    };

    // index buffer data
    std::vector<int> indices = {
        // Top
        0, 1, 2,
        0, 2, 3,

        // Left
        5, 4, 6,
        6, 4, 7,

        // Right
        8, 9, 10,
        8, 10, 11,

        // Front
        13, 12, 14,
        15, 14, 12,

        // Back
        16, 17, 18,
        16, 18, 19,

        // Bottom
        21, 20, 22,
        22, 20, 23
    };

    // create vertex array object
    glGenVertexArrays(1, &(mesh->m_VAO));
    glBindVertexArray(mesh->m_VAO);

    // generate->bind->assign data.
    glGenBuffers(1, &(mesh->m_VertexBufferObj));
    glBindBuffer(GL_ARRAY_BUFFER, mesh->m_VertexBufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);

    // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // generate index buffer
    glGenBuffers(1, &(mesh->m_IndexBufferObj));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_IndexBufferObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // unbind current buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    // Disable any open attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unsigned char* data = stbi_load("sample3b.png", &widthImg, &heightImg, &numColCh, STBI_rgb_alpha);
    if (!data) {
        std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
        exit(EXIT_FAILURE);

    }

    std::cout << "Image loaded successfully: " << widthImg << "x" << heightImg << std::endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void createGraphicsPipeline(Application* App) {
    // GLSL vertex and fragment shader source code stored in strings
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec3 position;\n"
        //"layout(location = 1) in vec3 vertexColors;\n"
        "layout(location = 1) in vec2 aTex;\n"

        "uniform mat4 u_ModelMatrix;\n"
        "uniform mat4 u_Perspective;\n"

        //"out vec3 v_VertexColors;\n"
        "out vec2 texCoord;\n"

        "void main()\n"
        "{\n"
        "   vec4 newPosition = u_Perspective * u_ModelMatrix * vec4(position, 1.0f);\n"
        // dont forget w!
        "   gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);\n"
        //"   v_VertexColors = vertexColors;\n"
        "   texCoord = aTex;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        //"in vec3 v_VertexColors;\n"
        "in vec2 texCoord;\n"
        "out vec4 fragColor;\n"
        "uniform sampler2D tex0;"
        "void main()\n"
        "{\n"
        "   fragColor = texture(tex0, texCoord);\n"
        "}\n";

    App->m_ShaderProgram = CreateShader(vertexShader, fragmentShader);
}

void meshUpdate(Mesh3D* mesh, Application* app, float dt) {
    glUseProgram(app->m_ShaderProgram);

    // enable depth test
    glEnable(GL_DEPTH_TEST);

    // accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glViewport(0, 0, app->m_ScreenWidth, app->m_ScreenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
;
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

    
    // create perspective matrix
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
    }
    else {
        std::cout << "Could not find location of u_Perspective." << std::endl;
        exit(EXIT_FAILURE);
    }

    // increase cube rotation with each frame
    if (mesh->m_uRotateDegrees >= 360) mesh->m_uRotateDegrees = 0;
    mesh->m_uRotateDegrees += 35 * dt;

    int texLocation = glGetUniformLocation(app->m_ShaderProgram, "tex0");
    glUniform1i(texLocation, 0);
}

void meshDraw(Mesh3D* mesh, unsigned int pipeline) {
    if (mesh == nullptr) return;

    // per mesh, choose pipeline to be used
    glUseProgram(pipeline);

    // bind texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // enable attributes
    glBindVertexArray(mesh->m_VAO);

    // render data
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // stop using current pipeline
    glUseProgram(0);
}

void meshDelete(Mesh3D* mesh) {
    glDeleteBuffers(1, &(mesh->m_VertexBufferObj));
    glDeleteVertexArrays(1, &(mesh->m_VAO));
}

void keyInput(Application* App, float dt) {
    if (glfwGetKey(App->m_Window, GLFW_KEY_UP) == GLFW_PRESS) {
        mesh1.m_uOffset += 5 * dt;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        mesh1.m_uOffset -= 5 * dt;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_E) == GLFW_PRESS) {
        if (mesh1.m_uScale <= 1.0f) mesh1.m_uScale += 2 * dt;
        else mesh1.m_uScale = 1.0f;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (mesh1.m_uScale >= 0.1f) mesh1.m_uScale -= 2 * dt;
        else mesh1.m_uScale = 0.1f;
    }
}

void mainLoop(Application* App) {
    // loop until the user closes the window
    float previousTime = 0.0;
    float currentTime = 0.0;
    float dt = 0.0;
    while (!glfwWindowShouldClose(App->m_Window)) {
        // delta time calculations
        currentTime = glfwGetTime();
        dt = currentTime - previousTime;
        previousTime = currentTime;

        // input
        keyInput(App, dt);

        // predraw
        meshUpdate(&mesh1, App, dt);

        // draw call
        meshDraw(&mesh1, App->m_ShaderProgram);

        // Swap front and back buffers
        glfwSwapBuffers(App->m_Window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

int cleanUp() {
    meshDelete(&mesh1);
    glDeleteProgram(App.m_ShaderProgram);
    glfwTerminate();
    return 0;
}

int main(void) {
    initializeProgram(&App);
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    vertexSpecification(&mesh1);

    createGraphicsPipeline(&App);

    mainLoop(&App);

    // delete all necesary components
    return cleanUp();
}