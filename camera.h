#ifndef _CAMERA_H
#define _CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>

struct Camera
{
    glm::vec3 cam_pos;
    glm::vec3 cam_dir;
    glm::vec3 cam_up;

    float pitch = 0.0f, yaw = 0.0f;

    glm::mat4 view;
    glm::mat4 projection;

    Camera();

    void eval();
};

#endif