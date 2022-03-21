#ifndef _MATERIAL_H
#define _MATERIAL_H

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

class Material
{
    std::map<std::string, Texture2D *> textures;
    std::map<std::string, int> properties_i;
    std::map<std::string, float> properties_f;
    std::map<std::string, glm::vec3> properties_v3;

    static std::map<std::string, Texture2D*> texture_pool;

    void loadTexturesAssimpType(aiTextureType type, const std::string &type_name, aiMaterial *mat, const std::string &dir = "");
    void loadTexturesAssimp(aiMaterial *mat, const std::string &dir = "");

public:
    Material();

    Material(aiMaterial *mat, const std::string &dir = "");

    void load(aiMaterial *mat, const std::string &dir = "");

    void use(Shader &shader);
};



#endif