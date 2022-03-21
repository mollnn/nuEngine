#include "model.h"

void Model::loadModelAssimp(const std::string &filename)
{
    Assimp::Importer importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    std::string dir = filename.substr(0, filename.find_last_of('/') + 1);
    std::function<void(aiNode *, const aiScene *)> solve = [&](aiNode *node, const aiScene *scene)
    {
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            meshes.push_back(Mesh(scene->mMeshes[node->mMeshes[i]], scene, dir));
        }
        for (int i = 0; i < node->mNumChildren; i++)
        {
            aiNode *child = node->mChildren[i];
            solve(child, scene);
        }
    };
    solve(scene->mRootNode, scene);
}

Model::Model()
{
}

Model::Model(const std::string &filename)
{
    loadModelAssimp(filename);
}

void Model::addChildren(std::shared_ptr<Model> model, const glm::mat4 &model_matrix)
{
    children.push_back({model, model_matrix});
}

void Model::draw(Shader &shader, const glm::mat4 &model_matrix)
{
    shader.setUniform("model", model_matrix);
    for (auto &i : meshes)
    {
        i.draw(shader);
    }
    for (auto &i : children)
    {
        i.first->draw(shader, i.second * model_matrix);
    }
}