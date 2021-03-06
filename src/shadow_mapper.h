#ifndef _SHADOW_MAPPER_H
#define _SHADOW_MAPPER_H

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
#include "texture_cube.h"

struct ShadowMapper
{
    const GLuint SHADOW_MAP_WIDTH = 512, SHADOW_MAP_HEIGHT = 512;
    Shader shadow_shader;
    TextureCube depth_texture;
    TextureCube pos_texture;
    TextureCube normal_texture;
    TextureCube flux_texture;  // out flux
    FramebufferObject shadow_map_fbo[6];
    float shadow_limit = 100.0;

    ShadowMapper();
    void lightPass(glm::vec3 shadow_light_pos,glm::vec3 shadow_light_int, Model *scene);
    void attach(Shader &shader);
};

#endif