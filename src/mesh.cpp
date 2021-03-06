#include "mesh.h"

void Mesh::loadMeshAssimp(aiMesh *mesh, const aiScene *scene, const std::string &dir)
{
    for (int j = 0; j < mesh->mNumVertices; j++)
    {
        vertices_.push_back(mesh->mVertices[j].x);
        vertices_.push_back(mesh->mVertices[j].y);
        vertices_.push_back(mesh->mVertices[j].z);
        vertices_.push_back(mesh->mNormals[j].x);
        vertices_.push_back(mesh->mNormals[j].y);
        vertices_.push_back(mesh->mNormals[j].z);
        if (mesh->mTextureCoords[0])
        {
            vertices_.push_back(mesh->mTextureCoords[0][j].x);
            vertices_.push_back(mesh->mTextureCoords[0][j].y);
        }
        else
        {
            vertices_.push_back(0);
            vertices_.push_back(0);
        }
        vertices_.push_back(mesh->mTangents[j].x);
        vertices_.push_back(mesh->mTangents[j].y);
        vertices_.push_back(mesh->mTangents[j].z);
        vertices_.push_back(mesh->mBitangents[j].x);
        vertices_.push_back(mesh->mBitangents[j].y);
        vertices_.push_back(mesh->mBitangents[j].z);
    }

    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
        {
            indices_.push_back(face.mIndices[j]);
        }
    }

    material_.load(scene->mMaterials[mesh->mMaterialIndex], dir);

    n_ = vertices_.size();
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(GLfloat), vertices_.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint), indices_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void *)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void *)(11 * sizeof(GLfloat)));
    glEnableVertexAttribArray(4);
}

Mesh::Mesh()
{
}

Mesh::Mesh(aiMesh *mesh, const aiScene *scene, const std::string &dir)
{
    loadMeshAssimp(mesh, scene, dir);
}

void Mesh::draw(Shader &shader)
{
    material_.use(shader);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    shader.solveTextures();
    glDrawElements(GL_TRIANGLES, n_, GL_UNSIGNED_INT, 0);
}