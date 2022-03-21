#ifndef _DEFERRED_H
#define _DEFERRED_H

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
#include "material.h"
#include "model.h"
#include "stb_image.h"
#include "framebuffer_object.h"

struct Deferred
{
    static const int n_gbuffer = 8;
    std::vector<GLfloat> vertices;
    GLuint vao, vbo;
    Shader gbuffer_shader;
    Texture2D gbuffer_texture[n_gbuffer];
    FramebufferObject gbuffer_fbo;
    int width_, height_;

    Deferred(int width, int height);
    void drawGeometry(Model *scene, const Camera &camera);
    void drawLighting(Shader &lighting_shader);
};

#endif