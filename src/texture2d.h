#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include "texture.h"

class Texture2D: public Texture
{
    std::string filename_;

    void genTexture();
    void setDefaultParams();

public:
    Texture2D(const std::string &tex_name = "");
    Texture2D(int width, int height, const void *data, GLuint intformat = GL_RGB, GLuint format = GL_RGB, GLuint dtype = GL_RGB);
    Texture2D(int width, int height);
    void setParami(GLuint k, GLuint v);
    virtual void use(int unit_id) override;
    virtual GLuint id() const override;
};

#endif