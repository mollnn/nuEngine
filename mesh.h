#ifndef _MESH_H
#define _MESH_H

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

class Mesh
{
    std::vector<GLfloat> vertices_;
    std::vector<GLuint> indices_;
    Material material_;

    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;
    int n_;

    void loadMeshAssimp(aiMesh *mesh, const aiScene *scene);

public:
    Mesh();

    Mesh(aiMesh *mesh, const aiScene *scene);

    void draw(Shader &shader);
};

#endif