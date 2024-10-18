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

void createGraphicsPipeline(Application *App) {
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

void meshUpdate(Mesh3D *mesh, Application *app, float dt) {
    // create model matrix and apply transformations
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), mesh->translate);
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
        100.0f);

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

void meshDelete(Mesh3D* mesh) {
    glDeleteBuffers(1, &(mesh->m_VertexBufferObj));
    glDeleteVertexArrays(1, &(mesh->m_VAO));
}

void keyInput(Application *App, Mesh3D *mesh, float dt) {
    if (glfwGetKey(App->m_Window, GLFW_KEY_UP) == GLFW_PRESS) {
        mesh->translate.y += 5 * dt;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        mesh->translate.y -= 5 * dt;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        mesh->translate.x += 5 * dt;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        mesh->translate.x -= 5 * dt;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_E) == GLFW_PRESS) {
        if (mesh->m_uScale <= 1.0f) mesh->m_uScale += 2 * dt;
        else mesh->m_uScale = 1.0f;
    }

    if (glfwGetKey(App->m_Window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (mesh->m_uScale >= 0.1f) mesh->m_uScale -= 2 * dt;
        else mesh->m_uScale = 0.1f;
    }
}

void mainLoop(Application *App, std::vector<Mesh3D*> *meshes) {
    // loop until the user closes the window
    float previousTime = 0.0;
    float currentTime = 0.0;
    float dt = 0.0;
    while (!glfwWindowShouldClose(App->m_Window)) {
        // delta time calculations
        currentTime = glfwGetTime();
        dt = currentTime - previousTime;
        previousTime = currentTime;

        glUseProgram(App->m_ShaderProgram);

        // enable depth test
        glEnable(GL_DEPTH_TEST);

        // accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        glViewport(0, 0, App->m_ScreenWidth, App->m_ScreenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input
        keyInput(App, meshes->at(0), dt);

        // draw meshes
        for (int i = 0; i < meshes->size(); i++) {
            meshUpdate(meshes->at(i), App, dt);
            meshes->at(i)->draw(App->m_ShaderProgram);
        }

        // Swap front and back buffers
        glfwSwapBuffers(App->m_Window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

int cleanUp(Application *App) {
    glDeleteProgram(App->m_ShaderProgram);
    glfwTerminate();
    return 0;
}

int main(void) {
    Application App = Application();
    Mesh3D mesh1 = Mesh3D();
    mesh1.translate.z = -10.0f;
    mesh1.translate.x = 2.0f;
    Mesh3D mesh2 = Mesh3D();
    mesh2.translate.z = -10.0f;
    mesh2.translate.x = -2.0f;
    Mesh3D mesh3 = Mesh3D();
    mesh3.translate.z = -10.0f;
    mesh3.translate.x = 4.0f;

    std::vector<Mesh3D*> meshes = std::vector<Mesh3D*>();
    meshes.push_back(&mesh1);
    meshes.push_back(&mesh2);
    meshes.push_back(&mesh3);

    createGraphicsPipeline(&App);

    mainLoop(&App, &meshes);

    // delete all necesary components
    return cleanUp(&App);
}