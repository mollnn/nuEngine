#ifndef _UNIFORM_BUFFER_H
#define _UNIFORM_BUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "pointlight.h"
#include "camera.h"
#include "cameracontrol.h"
#include "shader.h"
#include "texture2d.h"
#include "texture_cube.h"
#include "material.h"
#include "model.h"
#include "stb_image.h"

class UniformBuffer
{
    GLuint ubo;

public:
    UniformBuffer();
    void use(int id);
    void setData(int size, const void *value, GLuint mode);
};

#endif