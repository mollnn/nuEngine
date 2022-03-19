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
protected:
    GLuint handle_;

public:
    virtual void use(int unit_id) = 0;
    virtual GLuint id() const = 0;
};

#endif