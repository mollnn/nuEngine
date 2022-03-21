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

const int screen_x = 1920, screen_y = 1080;

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

    Scene scene;
    scene.add(std::make_shared<Model>("mitsuba.obj"));
    scene.add(std::make_shared<Model>("spot.obj"), glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f, 0.7f, 0.0f)));
    scene.add(std::make_shared<PointLight>(glm::vec3(500.0f, 500.0f, 500.0f), glm::vec3(-15.0f, 10.0f, 0.0f), 1.0f));
    scene.add(std::make_shared<PointLight>(glm::vec3(50.0f, 30.1f, 0.1f), glm::vec3(0.5f, 0.3f, 10.0f), 1.0f));
    scene.ambient_light_irradiance = glm::vec3(0.10f, 0.12f, 0.15f);

    SceneDesc scene_desc(scene);

    Camera main_camera;
    main_camera.aspect = 1.0f * screen_x / screen_y;

    CameraControl camera_control;
    camera_control.bind(&main_camera);

    ShadowMapper shadow_map_point_light;

    Deferred deferred(screen_x, screen_y);

    std::vector<float> rnds;
    std::uniform_real_distribution<GLfloat> random_float(0.0, 1.0);
    std::default_random_engine generator;
    std::vector<float> screen_rnd;

    for (int i = 0; i < screen_x * screen_y; i++)
    {
        screen_rnd.push_back(random_float(generator));
    }
    Texture2D screen_rnd_tex(screen_x, screen_y, screen_rnd.data(), GL_R16F, GL_RED, GL_FLOAT);

    for (int i = 0; i < 1024; i++)
    {
        rnds.push_back(random_float(generator));
    }

    GLuint ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 4096, rnds.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);

    Shader ssao_shader("../ssao.vs", "../ssao.fs");
    Texture2D ssao_texture(screen_x, screen_y);
    FramebufferObject ssao_fbo({&ssao_texture}, screen_x, screen_y);

    Shader lighting_shader("../lighting.vs", "../lighting.fs");
    Texture2D lighting_tex(screen_x, screen_y);
    FramebufferObject lighting_fbo({&lighting_tex}, screen_x, screen_y);

    Shader rsm_shader("../rsm.vs", "../rsm.fs");
    Texture2D rsm_tex(screen_x, screen_y);
    FramebufferObject rsm_fbo({&rsm_tex}, screen_x, screen_y);

    Shader ssr_shader("../ssr.vs", "../ssr.fs");
    Texture2D ssr_tex(screen_x, screen_y);
    FramebufferObject ssr_fbo({&ssr_tex}, screen_x, screen_y);

    Shader post_shader("../post.vs", "../post.fs");

    Profiler profiler;

    lighting_shader.use();
    lighting_shader.setTexture("screen_rnd_tex", &screen_rnd_tex);
    lighting_shader.setUniblock("ub_common", 1);
    shadow_map_point_light.attach(lighting_shader);

    ssao_shader.use();
    ssao_shader.setTexture("screen_rnd_tex", &screen_rnd_tex);
    ssao_shader.setUniblock("ub_common", 1);

    rsm_shader.use();
    rsm_shader.setTexture("screen_rnd_tex", &screen_rnd_tex);
    rsm_shader.setUniblock("ub_common", 1);
    shadow_map_point_light.attach(rsm_shader);

    ssr_shader.use();
    ssr_shader.setTexture("screen_rnd_tex", &screen_rnd_tex);
    ssr_shader.setUniblock("ub_common", 1);

    lighting_shader.setTexture("ao", &ssao_texture);
    ssr_shader.setTexture("film", &lighting_tex);
    post_shader.setTexture("lighting", &lighting_tex);
    post_shader.setTexture("rsm", &rsm_tex);
    post_shader.setTexture("ssr", &ssr_tex);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        camera_control.onEvents();
        profiler.begin();

        // SHADOW STAGE
        shadow_map_point_light.lightPass(scene_desc.point_lights[0].position, scene_desc.point_lights[0].intensity, &scene_desc.models);
        profiler.tick("shadow");
        glViewport(0, 0, screen_x, screen_y);

        // GEOMETRY STAGE
        deferred.drawGeometry(&scene_desc.models, main_camera);
        profiler.tick("geometry");

        // SSAO STAGE
        ssao_fbo.use();
        ssao_shader.use();
        ssao_shader.setCamera(main_camera);
        deferred.drawLighting(ssao_shader);
        profiler.tick("ssao.ao");

        // LIGHTING STAGE
        lighting_fbo.use();
        lighting_shader.use();
        lighting_shader.setLights(scene_desc.point_lights);
        lighting_shader.setUniform("ambient", scene_desc.ambient_light_irradiance);
        lighting_shader.setCamera(main_camera);
        deferred.drawLighting(lighting_shader);
        profiler.tick("lighting");

        // RSM STAGE
        rsm_fbo.use();
        rsm_shader.use();
        rsm_shader.setLights(scene_desc.point_lights);
        rsm_shader.setUniform("ambient", scene_desc.ambient_light_irradiance);
        rsm_shader.setCamera(main_camera);
        deferred.drawLighting(rsm_shader);
        profiler.tick("reflectsm");

        // SSR STAGE
        ssr_fbo.use();
        ssr_shader.use();
        ssr_shader.setCamera(main_camera);
        deferred.drawLighting(ssr_shader);
        profiler.tick("ssreflect");

        // POST PROCESSING
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        deferred.drawLighting(post_shader);
        profiler.tick("postproc");

        glfwSwapBuffers(window);
        profiler.end();
    }

    glfwTerminate();
    return 0;
}