#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>

class Texture2D
{
    GLuint handle_;
    std::string filename_;

    void genTexture();
    void setDefaultParams();

public:
    Texture2D(const std::string &tex_name = "");
    Texture2D(int width, int height, const void *data, GLuint intformat = GL_RGB, GLuint format = GL_RGB, GLuint dtype = GL_RGB);
    void setParami(GLuint k, GLuint v);
    void use(int unit_id);
    GLuint id();
};

#endif