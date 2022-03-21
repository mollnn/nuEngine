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

#include "texture2d.h"
#include "shader.h"
#include "material.h"
#include "mesh.h"
#include "object.h"

class Model: public Object
{
    std::vector<Mesh> meshes;
    void loadModelAssimp(const std::string &filename);

public:
    std::vector<std::pair<std::shared_ptr<Model>, glm::mat4>> children;
    Model();

    Model(const std::string &filename);
    void addChildren(std::shared_ptr<Model> model, const glm::mat4 &offset = glm::mat4(1.0f));
    void draw(Shader &shader, const glm::mat4 &model_matrix = glm::mat4(1.0f));
};

#endif