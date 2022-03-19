#ifndef _SHADER_H
#define _SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include "camera.h"
#include "pointlight.h"
#include "texture.h"

class Shader
{
    int sp_;
    static GLuint loadShader(const std::string &vs_name, const std::string &fs_name);

    std::map<std::string, Texture*> textures;

public:
    Shader(const std::string &vs_name, const std::string &fs_name);
    void use();
    void setUniformi(const std::string &name, int x);
    void setUniform(const std::string &name, float x);
    void setUniform(const std::string &name, float x, float y, float z);
    void setUniform(const std::string &name, const glm::vec3 &value);
    void setUniform(const std::string &name, const glm::mat4 &value);
    void setLights(const std::vector<PointLight> &lights);
    void setMVP(const glm::mat4 &view, const glm::mat4 &projection);
    void setCamera(const Camera &camera);
    void setTexture(const std::string& name, Texture* texture);
    void solveTextures();
    GLuint id();
};

#endif