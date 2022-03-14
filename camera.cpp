#include "camera.h"

Camera::Camera()
{
    cam_pos.x = 4;
    cam_pos.z = 4;
    cam_dir.z = -1;
    cam_up.y = 1;
    eval();
}

void Camera::eval()
{
    view = glm::lookAt(cam_pos, cam_pos + cam_dir, cam_up);
    projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);
}