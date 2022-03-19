#ifndef _FRAMEBUFFER_OBJECT_H
#define _FRAMEBUFFER_OBJECT_H

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


class FramebufferObject
{
    GLuint rbo_;
    int width_;
    int height_;
    std::vector<GLuint> atts;

public:
    GLuint fb_;
    FramebufferObject(const std::vector<Texture2D *> &attachments, int width, int height, const Texture2D* depth_attachments = nullptr);
    FramebufferObject(const std::vector<TextureCube *> &attachments, int width, int height, int cube_idx, const TextureCube* depth_attachments = nullptr);

    void use();
};

#endif