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

bool key_status[512];
float mouse_x, mouse_y;
bool mouse_button_status[3];

void framesizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS)
    {
        key_status[key] = true;
    }
    if (action == GLFW_RELEASE)
    {
        key_status[key] = false;
    }
}

void cursorCallback(GLFWwindow *window, double mx, double my)
{
    mouse_x = mx;
    mouse_y = my;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            mouse_button_status[0] = 1;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            mouse_button_status[1] = 1;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mouse_button_status[2] = 1;
            break;
        default:
            return;
        }
    }
    if (action == GLFW_RELEASE)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            mouse_button_status[0] = 0;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            mouse_button_status[1] = 0;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mouse_button_status[2] = 0;
            break;
        default:
            return;
        }
    }
    return;
}

int main()
{
    std::cout << "Nightup Engine v0.1" << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(512, 512, "Nightup Engine v0.1", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed at glfwCreateWindow" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framesizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);

    Model m0("mitsuba.obj");
    Shader shader("../shader.vs", "../shader.fs");
    Shader shadow_shader("../shadow.vs", "../shadow.fs");
    Camera camera;
    CameraControl cam_control;
    cam_control.bind(&camera);

    const GLuint SHADOW_MAP_WIDTH = 1024, SHADOW_MAP_HEIGHT = 1024;
    GLuint shadow_map_fbo;
    glGenFramebuffers(1, &shadow_map_fbo);
    GLuint shadow_map_texture;
    glGenTextures(1, &shadow_map_texture);
    glBindTexture(GL_TEXTURE_2D, shadow_map_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat texture_border_color[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, texture_border_color);

    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        cam_control.onEvents();

        shadow_shader.use();
        glm::mat4 light_view, light_proj, light_vp;
        light_view = glm::lookAt(glm::vec3(-10.0f, 15.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0, 0.0, 0.0));
        light_proj = glm::perspective(glm::radians(30.0), 1.0, 10.0, 100.0);
        light_vp = light_proj * light_view;

        shadow_shader.setUniform("lvp", (light_vp));
        shadow_shader.setUniform("model", (glm::mat4(1.0f)));

        glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        m0.draw(shadow_shader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 512, 512);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.setLights({PointLight(glm::vec3(500.0f, 500.0f, 500.0f), glm::vec3(-10.0f, 15.0f, 0.0f)),
                          PointLight(glm::vec3(50.0f, 30.1f, 0.1f), glm::vec3(0.5f, 0.3f, 10.0f))});
        shader.setCamera(camera);
        shader.setUniform("model", (glm::mat4(1.0f)));
        shader.setUniform("lvp", (light_vp));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadow_map_texture);

        m0.draw(shader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}