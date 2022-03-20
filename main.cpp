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
#include "scenedesc.h"

const int screen_x = 640, screen_y = 360;

extern bool key_status[512];
extern float mouse_x, mouse_y;
extern bool mouse_button_status[3];
GLFWwindow *window;

void init()
{
    std::cout << "Nightup Engine v0.1" << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screen_x, screen_y, "Nightup Engine v0.1", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed at glfwCreateWindow" << std::endl;
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framesizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


int main()
{
    init();

    auto scene = std::make_shared<Object>();
    scene->addChildren(std::make_shared<Model>("mitsuba.obj"));
    scene->addChildren(std::make_shared<Model>("spot.obj"), glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.7f, 0.0f)));
    scene->addChildren(std::make_shared<PointLight>(glm::vec3(500.0f, 500.0f, 500.0f), glm::vec3(-15.0f, 10.0f, 0.0f)));
    scene->addChildren(std::make_shared<PointLight>(glm::vec3(50.0f, 30.1f, 0.1f), glm::vec3(0.5f, 0.3f, 10.0f)));

    SceneDesc scene_desc(scene);
    scene_desc.ambient_light_irradiance = glm::vec3(0.2f, 0.2f, 0.2f);

    Camera main_camera;
    main_camera.aspect = 1.0f * screen_x / screen_y;

    CameraControl camera_control;
    camera_control.bind(&main_camera);

    ShadowMapper shadow_map_point_light;

    Shader lighting_shader("../lighting.vs", "../lighting.fs");

    Shader ssao_shader("../ssao.vs", "../ssao.fs");
    Deferred deferred_renderer(screen_x, screen_y);

    std::vector<float> rnds;
    std::uniform_real_distribution<GLfloat> random_float(0.0, 1.0);
    std::default_random_engine generator;

    for (int i = 0; i < 1024; i++)
    {
        rnds.push_back(random_float(generator));
    }

    std::vector<float> screen_rnd;
    for (int i = 0; i < screen_x * screen_y * 3; i++)
    {
        screen_rnd.push_back(random_float(generator));
    }

    Texture2D screen_rnd_tex(screen_x, screen_y, screen_rnd.data(), GL_RGB16F, GL_RGB, GL_FLOAT);

    Texture2D ssao_texture(screen_x, screen_y);
    FramebufferObject ssao_fbo({&ssao_texture}, screen_x, screen_y);

    Texture2D film_tex(screen_x, screen_y);
    FramebufferObject film_fbo({&film_tex}, screen_x, screen_y);

    Shader ssr_shader("../ssr.vs", "../ssr.fs");

    Profiler profiler;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        camera_control.onEvents();

        profiler.begin();

        // GEN SHADOW
        shadow_map_point_light.lightPass(scene_desc.point_lights[0].position, scene_desc.point_lights[0].intensity, &scene_desc.models);
        profiler.tick("shadow");

        glViewport(0, 0, screen_x, screen_y);

        // GEOMETRY STAGE
        deferred_renderer.drawGeometry(&scene_desc.models, main_camera);
        profiler.tick("geometry");

        // SSAO STAGE
        ssao_fbo.use();
        ssao_shader.use();
        ssao_shader.setCamera(main_camera);
        ssao_shader.setTexture("screen_rnd_tex", &screen_rnd_tex);

        for (int i = 0; i < 256; i++)
        {
            ssao_shader.setUniform("rnds[" + std::to_string(i) + "]", rnds[i]);
        }
        deferred_renderer.drawLighting(ssao_shader);

        profiler.tick("ssao.ao");

        // LIGHTING STAGE
        film_fbo.use();
        lighting_shader.use();
        lighting_shader.setLights(scene_desc.point_lights);
        lighting_shader.setCamera(main_camera);
        lighting_shader.setTexture("screen_rnd_tex", &screen_rnd_tex);
        lighting_shader.setUniform("ambient", scene_desc.ambient_light_irradiance);
        for (int i = 0; i < 256; i++)
        {
            lighting_shader.setUniform("rnds[" + std::to_string(i) + "]", rnds[i]);
        }
        shadow_map_point_light.attach(lighting_shader);
        lighting_shader.setTexture("ao", &ssao_texture);
        deferred_renderer.drawLighting(lighting_shader);

        profiler.tick("lighting");

        // SSR STAGE
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ssr_shader.use();
        ssr_shader.setCamera(main_camera);
        ssr_shader.setTexture("film", &film_tex);
        ssr_shader.setTexture("screen_rnd_tex", &screen_rnd_tex);

        for (int i = 0; i < 256; i++)
        {
            ssr_shader.setUniform("rnds[" + std::to_string(i) + "]", rnds[i]);
        }
        deferred_renderer.drawLighting(ssr_shader);

        profiler.tick("ssreflect");
        glfwSwapBuffers(window);

        profiler.end();
    }

    glfwTerminate();
    return 0;
}