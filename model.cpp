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
    std::function<void(aiNode *, const aiScene *)> solve = [&](aiNode *node, const aiScene *scene)
    {
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            meshes.push_back(Mesh(scene->mMeshes[node->mMeshes[i]], scene));
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

void Model::draw(Shader &shader)
{
    for (auto &i : meshes)
    {
        i.draw(shader);
    }
}