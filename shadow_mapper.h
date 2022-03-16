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
#include "texture.h"
#include "material.h"
#include "model.h"
#include "stb_image.h"

struct ShadowMapper
{
    const GLuint SHADOW_MAP_WIDTH = 2048, SHADOW_MAP_HEIGHT = 2048;
    Shader shadow_shader;
    GLuint shadow_map_fbo;
    GLuint shadow_map_texture;
    float shadow_limit = 100.0;

    ShadowMapper();
    void lightPass(glm::vec3 shadow_light_pos, Model *scene);
    void attach(Shader &shader);
};

#endif