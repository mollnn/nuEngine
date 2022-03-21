#include "cameracontrol.h"

extern bool key_status[512];
extern float mouse_x, mouse_y;
extern bool mouse_button_status[3];

void CameraControl::bind(Camera *c)
{
    camera = c;
}

void CameraControl::onEvents()
{
    if (camera == nullptr)
    {
        return;
    }

    glm::vec3 cam_pos = camera->cam_pos;
    glm::vec3 cam_dir = camera->cam_dir;
    glm::vec3 cam_up = camera->cam_up;

    float pitch = camera->pitch;
    float yaw = camera->yaw;

    static GLfloat last_frame_time = glfwGetTime();
    static GLfloat last_cursor_x = mouse_x, last_cursor_y = mouse_y;

    GLfloat current_time = glfwGetTime();

    GLfloat delta_cursor_x = mouse_x - last_cursor_x;
    GLfloat delta_cursor_y = mouse_y - last_cursor_y;

    last_cursor_x = mouse_x;
    last_cursor_y = mouse_y;

    GLfloat cam_speed = 1.0f;
    GLfloat cam_pan_speed = 0.005f;
    GLfloat cam_rotate_speed = 0.2f;

    if (key_status[GLFW_KEY_LEFT_ALT] && mouse_button_status[0])
    {
        pitch += -89.0f * delta_cursor_y * cam_rotate_speed / 512;
        yaw += 1080.0f * delta_cursor_x * cam_rotate_speed / 512;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    cam_dir.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
    cam_dir.y = glm::sin(glm::radians(pitch));
    cam_dir.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));
    cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cam_hand = glm::cross(cam_dir, cam_up);
    cam_up = glm::cross(cam_hand, cam_dir);
    cam_up = glm::normalize(cam_up);
    cam_hand = glm::cross(cam_dir, cam_up);

    if (key_status[GLFW_KEY_LEFT_CONTROL] && mouse_button_status[0])
    {
        cam_pos += cam_hand * delta_cursor_x * cam_pan_speed;
        cam_pos -= cam_up * delta_cursor_y * cam_pan_speed;
    }

    if (key_status[GLFW_KEY_LEFT_SHIFT] && mouse_button_status[0])
    {
        cam_pos += cam_hand * delta_cursor_x * cam_pan_speed;
        cam_pos -= cam_dir * delta_cursor_y * cam_pan_speed;
    }

    if (key_status[GLFW_KEY_UP])
    {
        cam_pos += cam_dir * cam_speed * (current_time - last_frame_time);
    }
    if (key_status[GLFW_KEY_DOWN])
    {
        cam_pos -= cam_dir * cam_speed * (current_time - last_frame_time);
    }
    if (key_status[GLFW_KEY_LEFT])
    {
        cam_pos -= cam_hand * cam_speed * (current_time - last_frame_time);
    }
    if (key_status[GLFW_KEY_RIGHT])
    {
        cam_pos += cam_hand * cam_speed * (current_time - last_frame_time);
    }
    last_frame_time = current_time;

    camera->cam_pos = cam_pos;
    camera->cam_dir = cam_dir;
    camera->cam_up = cam_up;

    camera->yaw = yaw;
    camera->pitch = pitch;

    camera->eval();
}