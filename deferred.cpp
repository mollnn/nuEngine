#include "deferred.h"

Deferred::Deferred(int width, int height) : width_(width), height_(height), gbuffer_shader("../gbuf.vs", "../gbuf.fs"),
                                            gbuffer_fbo(std::vector<Texture2D *>({gbuffer_texture + 0,
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

void Deferred::drawGeometry(Model *scene, const Camera &camera)
{
    gbuffer_fbo.use();
    gbuffer_shader.use();
    gbuffer_shader.setCamera(camera);
    scene->draw(gbuffer_shader);
}

void Deferred::drawLighting(Shader &lighting_shader)
{
    // set your fbo before calling this function
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