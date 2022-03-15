#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>

class Texture
{
    int handle_;
    std::string filename_;

    static GLuint loadTexture(const std::string &tex_name);

public:
    Texture(const std::string &tex_name = "");

    void use(int unit_id);
    int id();
};

#endif