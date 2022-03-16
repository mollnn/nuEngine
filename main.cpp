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
#include "framebuffer_object.h"
#include "deferred.h"
#include "shadow_mapper.h"

bool key_status[512];
float mouse_x, mouse_y;
bool mouse_button_status[3];
const int screen_x = 1280, screen_y = 720;

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

    GLFWwindow *window = glfwCreateWindow(screen_x, screen_y, "Nightup Engine v0.1", nullptr, nullptr);
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

    Model scene;
    scene.addChildren(std::make_shared<Model>("mitsuba.obj"));
    scene.addChildren(std::make_shared<Model>("spot.obj"), glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.7f, 0.0f)));
    Camera camera;
    camera.aspect = 1.0f * screen_x / screen_y;
    CameraControl cam_control;
    cam_control.bind(&camera);

    ShadowMapper shadow_map_point_light;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Shader lighting_shader("../lighting.vs", "../lighting.fs");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    Shader ssao_shader("../ssao.vs", "../ssao.fs");
    Deferred deferred_renderer(screen_x, screen_y);

    std::vector<glm::vec3> ssao_kernel;
    std::normal_distribution<GLfloat> random_float(0.0, 1.0);
    std::default_random_engine generator;

    for (int i = 0; i < 64; i++)
    {
        glm::vec3 sample(random_float(generator), random_float(generator), random_float(generator));
        sample = glm::normalize(sample);
        sample *= random_float(generator);
        ssao_kernel.push_back(sample);
    }

    Texture ssao_texture;
    FramebufferObject ssao_fbo({&ssao_texture}, screen_x, screen_y);

    std::vector<PointLight> lights = {PointLight(glm::vec3(500.0f, 500.0f, 500.0f), glm::vec3(-10.0f, 10.0f, 0.0f)),
                                      PointLight(glm::vec3(50.0f, 30.1f, 0.1f), glm::vec3(0.5f, 0.3f, 10.0f))};

    glm::vec3 ambient_light_irradiance(0.3f, 0.3f, 0.3f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        cam_control.onEvents();

        scene.children[1].second = glm::rotate(scene.children[1].second, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));

        // GEN SHADOW
        shadow_map_point_light.lightPass(lights[0].position, &scene);

        glViewport(0, 0, screen_x, screen_y);

        // GEOMETRY STAGE
        deferred_renderer.drawGeometry(&scene, camera);

        // SSAO STAGE
        ssao_fbo.use();
        ssao_shader.use();
        ssao_shader.setCamera(camera);
        for (int i = 0; i < 32; i++)
        {
            ssao_shader.setUniform("ssao_kernel[" + std::to_string(i) + "]", ssao_kernel[i]);
        }
        deferred_renderer.drawLighting(ssao_shader);

        // LIGHTING STAGE
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lighting_shader.use();
        lighting_shader.setLights(lights);
        lighting_shader.setUniform("ambient", ambient_light_irradiance);

        shadow_map_point_light.attach(lighting_shader);
        lighting_shader.setTexture("ao", &ssao_texture);
        deferred_renderer.drawLighting(lighting_shader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}