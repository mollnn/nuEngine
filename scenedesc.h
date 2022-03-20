#ifndef _SCENE_DESC_H
#define _SCENE_DESC_H

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
#include "deferred.h"
#include "shadow_mapper.h"
#include "profiler.h"
#include "callbacks.h"


struct SceneDesc
{
    Model models;
    glm::vec3 ambient_light_irradiance;
    std::vector<PointLight> point_lights;

    SceneDesc();

    SceneDesc(std::shared_ptr<Object> obj);
};


#endif