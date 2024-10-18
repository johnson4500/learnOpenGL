#define STB_IMAGE_IMPLEMENTATION
#include <Mesh3D.hpp>
#include <stb_image.h>
#include <iostream>

Mesh3D::Mesh3D() {
	this->vertices = {
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

	this->indices = {
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

	setupMesh();
}

void Mesh3D::setupMesh() {
    // create vertex array object
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // generate->bind->assign data.
    glGenBuffers(1, &m_VertexBufferObj);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // generate index buffer
    glGenBuffers(1, &m_IndexBufferObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // unbind current buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // texture loading
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_set_flip_vertically_on_load(true);
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
    glBindVertexArray(0);
}

void Mesh3D::draw(unsigned int pipeline) {
    // per mesh, choose pipeline to be used
    glUseProgram(pipeline);

    // bind texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // stop using current pipeline
    //glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);
}