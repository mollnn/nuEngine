#ifndef _SCENE_H_
#define _SCENE_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include "object.h"

struct Scene
{
    std::shared_ptr<Object> root;
    glm::vec3 ambient_light_irradiance;

    Scene();

    void add(std::shared_ptr<Object> obj, const glm::mat4 &offset = glm::mat4(1.0f));
};

#endif