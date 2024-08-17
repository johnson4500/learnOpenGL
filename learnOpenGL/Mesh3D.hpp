#ifndef MESH3D_HPP
#define MESH3D_HPP
#include <GL/glew.h>
struct Mesh3D {
    // vertex array object and buffer variables
    unsigned int m_VAO;
    unsigned int m_VertexBufferObj;
    unsigned int m_ColorBufferObj;
    unsigned int m_IndexBufferObj;
    unsigned int m_Pipeline = 0;

    GLfloat m_uOffset = -1.0f;
    GLfloat m_uScale = 1.0f;
    GLfloat m_uRotateDegrees = 0.0f;

};

#endif