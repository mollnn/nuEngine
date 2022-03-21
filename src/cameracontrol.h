#ifndef _CAMERACONTROL_H
#define _CAMERACONTROL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>

#include "camera.h"

struct CameraControl
{
    Camera *camera = nullptr;

    void bind(Camera *c);

    void onEvents();
};

#endif