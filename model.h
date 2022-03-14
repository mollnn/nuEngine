#ifndef _MODEL_H
#define _MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "texture.h"
#include "shader.h"
#include "material.h"
#include "mesh.h"

class Model
{
    std::vector<Mesh> meshes;

    void loadModelAssimp(const std::string &filename);

public:
    Model();

    Model(const std::string &filename);

    void draw(Shader &shader);
};

#endif