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
#include "texture.h"
#include "material.h"
#include "model.h"
#include "stb_image.h"


class FramebufferObject
{
    GLuint fb_;
    GLuint rbo_;

public:
    FramebufferObject(const std::vector<Texture *> &attachments, int width, int height);

    void use();
};

#endif