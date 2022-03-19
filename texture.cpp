#include "texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void Texture::setDefaultParams()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(const std::string &tex_name)
{
    int sx, sy;

    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_2D, handle_);
    setDefaultParams();

    if (tex_name != "")
    {
        unsigned char *tex_data = stbi_load(tex_name.c_str(), &sx, &sy, nullptr, 3);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sx, sy, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
    }
    filename_ = tex_name;
}

Texture::Texture(int width, int height, const void *data, GLuint intformat, GLuint format, GLuint dtype)
{
    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_2D, handle_);
    setDefaultParams();

    glTexImage2D(GL_TEXTURE_2D, 0, intformat, width, height, 0, format, dtype, data);
}

void Texture::setParami(GLuint k, GLuint v)
{
    glBindTexture(GL_TEXTURE_2D, handle_);
    glTexParameteri(GL_TEXTURE_2D, k, v);
}

void Texture::use(int unit_id)
{
    glActiveTexture(GL_TEXTURE0 + unit_id);
    glBindTexture(GL_TEXTURE_2D, handle_);
}

int Texture::id()
{
    return handle_;
}
