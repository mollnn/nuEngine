#include "material.h"

void Material::loadTexturesAssimpType(aiTextureType type, const std::string &type_name, aiMaterial *mat, const std::string &dir)
{
    for (int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString filename;
        mat->GetTexture(type, i, &filename);
        Texture2D *texture = new Texture2D(filename.C_Str());
        textures[type_name + std::to_string(i + 1)] = texture;
        std::string flag_name = "usetex" + type_name.substr(7);
        properties_i[flag_name] = 1;
    }
}

void Material::loadTexturesAssimp(aiMaterial *mat, const std::string &dir)
{
    float t;
    if (AI_SUCCESS != mat->Get(AI_MATKEY_SHININESS, t))
        t = 1.0f;
    properties_f["shininess"] = t;

    glm::vec3 tv3;
    aiColor3D aitv3;

    if (AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_AMBIENT, aitv3))
        tv3 = glm::vec3(0.0f);
    else
        tv3.r = aitv3.r, tv3.g = aitv3.g, tv3.b = aitv3.b;
    properties_v3["color_ambient"] = tv3;
    properties_i["usetex_ambient"] = 0;

    if (AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_DIFFUSE, aitv3))
        tv3 = glm::vec3(0.5f);
    else
        tv3.r = aitv3.r, tv3.g = aitv3.g, tv3.b = aitv3.b;
    properties_v3["color_diffuse"] = tv3;
    properties_i["usetex_diffuse"] = 0;

    if (AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_SPECULAR, aitv3))
        tv3 = glm::vec3(0.5f);
    else
        tv3.r = aitv3.r, tv3.g = aitv3.g, tv3.b = aitv3.b;
    properties_v3["color_specular"] = tv3;
    properties_i["usetex_specular"] = 0;

    loadTexturesAssimpType(aiTextureType_AMBIENT, "texture_ambient", mat, dir);
    loadTexturesAssimpType(aiTextureType_DIFFUSE, "texture_diffuse", mat, dir);
    loadTexturesAssimpType(aiTextureType_NORMALS, "texture_normals", mat, dir);
    loadTexturesAssimpType(aiTextureType_SPECULAR, "texture_specular", mat, dir);
}

Material::Material()
{
}

Material::Material(aiMaterial *mat, const std::string &dir)
{
    // load from material file, directly load textures
    loadTexturesAssimp(mat, dir);
}

void Material::load(aiMaterial *mat, const std::string &dir)
{
    // load from material file, directly load textures
    loadTexturesAssimp(mat, dir);
}

void Material::use(Shader &shader)
{
    // put textures into shader
    GLuint shader_id = shader.id();
    GLuint texture_unit_id = 4; // first 4 units are reserved
    for (GLuint i = texture_unit_id; i < texture_unit_id + 8; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    for (auto &[k, v] : textures)
    {
        shader.setUniformi(k, texture_unit_id);
        v->use(texture_unit_id);
        texture_unit_id++;
    }
    for (auto &[k, v] : properties_f)
    {
        shader.setUniform(k, v);
    }
    for (auto &[k, v] : properties_i)
    {
        shader.setUniformi(k, v);
    }
    for (auto &[k, v] : properties_v3)
    {
        shader.setUniform(k, v);
    }
}