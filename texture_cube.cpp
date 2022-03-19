#include "texture_cube.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void TextureCube::setDefaultParams()
{
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

TextureCube::TextureCube(int width, int height, GLuint intformat, GLuint format, GLuint dtype)
{
    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);
    setDefaultParams();
    for (int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, intformat, width, height, 0, format, dtype, nullptr);
}

void TextureCube::setParami(GLuint k, GLuint v)
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, k, v);
}

void TextureCube::use(int unit_id)
{
    glActiveTexture(GL_TEXTURE0 + unit_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle_);
}

GLuint TextureCube::id() const
{
    return handle_;
}
