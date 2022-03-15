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

struct ShadowMapperPoint
{
    const GLuint SHADOW_MAP_WIDTH = 1024, SHADOW_MAP_HEIGHT = 1024;
    Shader shadow_shader;
    GLuint shadow_map_fbo;
    GLuint shadow_map_texture;
    float shadow_limit = 100.0;

    ShadowMapperPoint() : shadow_shader("../shadow.vs", "../shadow.fs")
    {
        glGenFramebuffers(1, &shadow_map_fbo);
        glGenTextures(1, &shadow_map_texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map_texture);
        for (int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat texture_border_color[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, texture_border_color);

        glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void lightPass(glm::vec3 shadow_light_pos, Model *scene)
    {
        shadow_shader.use();
        glm::mat4 shadow_light_view[6], shadow_light_projection;
        shadow_light_view[0] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0, 0.0));
        shadow_light_view[1] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0, 0.0));
        shadow_light_view[2] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0, 0.0, 1.0));
        shadow_light_view[3] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0, 0.0, -1.0));
        shadow_light_view[4] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0, -1.0, 0.0));
        shadow_light_view[5] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0, -1.0, 0.0));
        shadow_light_projection = glm::perspective(glm::radians(90.0), 1.0, 1.0, 100.0);

        shadow_shader.setUniform("model", (glm::mat4(1.0f)));
        shadow_shader.setUniform("lightPos", shadow_light_pos);
        shadow_shader.setUniform("shadowLimit", shadow_limit);

        for (int i = 0; i < 6; i++)
        {
            shadow_shader.setUniform("lvp", shadow_light_projection * shadow_light_view[i]);
            glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, shadow_map_texture, 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            scene->draw(shadow_shader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void attach(Shader &shader)
    {
        shader.setUniform("shadowLimit", shadow_limit);
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map_texture);
        shader.setUniformi("shadow_map", 15);
    }
};

class FramebufferObject
{
    GLuint fb_;
    GLuint rbo_;

public:
    FramebufferObject(const std::vector<Texture *> &attachments, int width, int height)
    {
        glGenFramebuffers(1, &fb_);
        glBindFramebuffer(GL_FRAMEBUFFER, fb_);

        std::vector<GLuint> atts;

        for (int i = 0; i < attachments.size(); i++)
        {
            atts.push_back(GL_COLOR_ATTACHMENT0 + i);
            glBindTexture(GL_TEXTURE_2D, attachments[i]->id());
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments[i]->id(), 0);
        }
        glDrawBuffers(attachments.size(), atts.data());

        glGenRenderbuffers(1, &rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_);
    }

    void use()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fb_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};

struct DeferredRenderer
{
    static const int n_gbuffer = 6;
    std::vector<GLfloat> vertices;
    GLuint vao, vbo;
    Shader gbuffer_shader;
    Texture gbuffer_texture[n_gbuffer];
    FramebufferObject gbuffer_fbo;
    int width_, height_;

    DeferredRenderer(int width, int height) : width_(width), height_(height), gbuffer_shader("../gbuf.vs", "../gbuf.fs"),
                                              gbuffer_fbo(std::vector<Texture *>({gbuffer_texture + 0,
                                                                                  gbuffer_texture + 1,
                                                                                  gbuffer_texture + 2,
                                                                                  gbuffer_texture + 3,
                                                                                  gbuffer_texture + 4,
                                                                                  gbuffer_texture + 5}),
                                                          width, height)
    {
        vertices = {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                    1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                    1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                    1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    void geometryPass(Model *scene, const Camera &camera)
    {
        gbuffer_fbo.use();
        gbuffer_shader.use();
        gbuffer_shader.setCamera(camera);
        scene->draw(gbuffer_shader);
    }

    void lightingPass(Shader &lighting_shader)
    {
        lighting_shader.use();
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        for (int i = 0; i < n_gbuffer; i++)
        {
            lighting_shader.setTexture("gbuf" + std::to_string(i), gbuffer_texture + i);
        }
        lighting_shader.solveTextures();
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
};

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

    Model scene;
    scene.addChildren(std::make_shared<Model>("mitsuba.obj"));
    scene.addChildren(std::make_shared<Model>("spot.obj"), glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.7f, 0.0f)));
    Camera camera;
    CameraControl cam_control;
    cam_control.bind(&camera);

    ShadowMapperPoint shadow_map_p;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Shader lighting_shader("../lighting.vs", "../lighting.fs");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    Shader ssao_shader("../ssao.vs", "../ssao.fs");
    DeferredRenderer deferred_renderer(512, 512);

    std::vector<glm::vec3> ssao_kernel;
    std::normal_distribution<GLfloat> random_float(0.0, 1.0);
    std::default_random_engine generator;
    for (int i = 0; i < 64; i++)
    {
        glm::vec3 sample(random_float(generator), random_float(generator), random_float(generator));
        ssao_kernel.push_back(sample);
    }

    Texture ssao_texture;
    FramebufferObject ssao_fbo({&ssao_texture}, 512, 512);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        cam_control.onEvents();

        scene.children[1].second = glm::rotate(scene.children[1].second, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));

        // GEN SHADOW
        shadow_map_p.lightPass(glm::vec3(-10.0f, 10.0f, 0.0f), &scene);

        glViewport(0, 0, 512, 512);

        // GEOMETRY STAGE
        deferred_renderer.geometryPass(&scene, camera);

        // AO STAGE
        ssao_fbo.use();
        ssao_shader.use();
        ssao_shader.setUniform("near", 0.1f);
        ssao_shader.setUniform("far", 100.0f);
        ssao_shader.setCamera(camera);
        for (int i = 0; i < 64; i++)
        {
            ssao_shader.setUniform("ssao_kernel[" + std::to_string(i) + "]", ssao_kernel[i]);
        }
        deferred_renderer.lightingPass(ssao_shader);

        // LIGHTING STAGE
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lighting_shader.use();
        lighting_shader.setLights({PointLight(glm::vec3(500.0f, 500.0f, 500.0f), glm::vec3(-10.0f, 10.0f, 0.0f)),
                                   PointLight(glm::vec3(50.0f, 30.1f, 0.1f), glm::vec3(0.5f, 0.3f, 10.0f))});
        lighting_shader.setUniform("ambient", glm::vec3(0.2f, 0.2f, 0.3f));

        shadow_map_p.attach(lighting_shader);
        lighting_shader.setTexture("ao", &ssao_texture);
        lighting_shader.solveTextures();
        deferred_renderer.lightingPass(lighting_shader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}