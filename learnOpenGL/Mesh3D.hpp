#include <GL/glew.h>
struct Mesh3D {
    // vertex array object and buffer variables
    unsigned int m_VAO;
    unsigned int m_BufferObj;
    unsigned int m_ColorBufferObj;
    unsigned int m_IndexBufferObj;

    GLfloat m_uOffset = -1.0f;
    GLfloat m_uScale = 1.0f;
    GLfloat m_uRotateDegrees = 0.0f;

};