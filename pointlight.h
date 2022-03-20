#ifndef _POINTLIGHT_H
#define _POINTLIGHT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "object.h"

struct PointLight: public Object
{
    glm::vec3 intensity;
    glm::vec3 position;

    PointLight(const glm::vec3 &i, const glm::vec3 &p);
};


#endif