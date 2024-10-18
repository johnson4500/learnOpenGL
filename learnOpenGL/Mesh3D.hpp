#ifndef MESH3D_HPP
#define MESH3D_HPP
#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

class Mesh3D {
    // vertex array object and buffer variables
    public:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        unsigned int m_VAO;
        unsigned int m_VertexBufferObj;
        unsigned int m_ColorBufferObj;
        unsigned int m_IndexBufferObj;
        unsigned int m_Pipeline = 0;
        unsigned int texture;
        int widthImg, heightImg, numColCh;
        GLfloat m_uOffset = -1.0f;
        GLfloat m_uScale = 1.0f;
        GLfloat m_uRotateDegrees = 0.0f;
        glm::vec3 translate = glm::vec3(0.0f, 0.0f, 0.0f);

        Mesh3D();
        Mesh3D(std::vector<float> vertices, std::vector<unsigned int> indices);
        void draw(unsigned int pipeline);

    private:
        void setupMesh();
};

#endif