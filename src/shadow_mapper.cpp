#include "shadow_mapper.h"

ShadowMapper::ShadowMapper() : shadow_shader("../shaders/shadow.vs", "../shaders/shadow.fs"),
                               depth_texture(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT),
                               pos_texture(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT),
                               normal_texture(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT),
                               flux_texture(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT),
                               shadow_map_fbo{
                                   {{&pos_texture, &normal_texture, &flux_texture}, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, &depth_texture},
                                   {{&pos_texture, &normal_texture, &flux_texture}, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 1, &depth_texture},
                                   {{&pos_texture, &normal_texture, &flux_texture}, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 2, &depth_texture},
                                   {{&pos_texture, &normal_texture, &flux_texture}, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 3, &depth_texture},
                                   {{&pos_texture, &normal_texture, &flux_texture}, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 4, &depth_texture},
                                   {{&pos_texture, &normal_texture, &flux_texture}, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 5, &depth_texture}}
{
    depth_texture.setParami(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    depth_texture.setParami(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    depth_texture.setParami(GL_GENERATE_MIPMAP, GL_FALSE);
    pos_texture.setParami(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    pos_texture.setParami(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    normal_texture.setParami(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    normal_texture.setParami(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    flux_texture.setParami(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    flux_texture.setParami(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glGenFramebuffers(1, &shadow_map_fbo);
    // glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);

    // auto makeCubemap = [&](GLuint &id, GLuint intformat, GLuint component)
    // {
    //     glGenTextures(1, &id);
    //     glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    //     for (int i = 0; i < 6; i++)
    //         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, intformat, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, component, GL_FLOAT, NULL);
    //     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     // GLfloat texture_border_color[] = {1.0, 1.0, 1.0, 1.0};
    //     // glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, texture_border_color);
    //     glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    // };

    // makeCubemap(depth_texture, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
    // makeCubemap(pos_texture, GL_RGB16F, GL_RGB);
    // makeCubemap(normal_texture, GL_RGB16F, GL_RGB);
    // makeCubemap(flux_texture, GL_RGB16F, GL_RGB);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapper::lightPass(glm::vec3 shadow_light_pos, glm::vec3 shadow_light_int, Model *scene)
{
    shadow_shader.use();
    glm::mat4 shadow_light_view[6], shadow_light_projection;
    shadow_light_view[0] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0, 0.0));
    shadow_light_view[1] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0, 0.0));
    shadow_light_view[2] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0, 0.0, 1.0));
    shadow_light_view[3] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0, 0.0, -1.0));
    shadow_light_view[4] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0, -1.0, 0.0));
    shadow_light_view[5] = glm::lookAt(shadow_light_pos, shadow_light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0, -1.0, 0.0));
    shadow_light_projection = glm::perspective(glm::radians(90.0), 1.0, 0.01, 100.0);

    shadow_shader.setUniform("model", (glm::mat4(1.0f)));
    shadow_shader.setUniform("light_pos", shadow_light_pos);
    shadow_shader.setUniform("light_int", shadow_light_int);
    shadow_shader.setUniform("shadow_limit", shadow_limit);

    for (int i = 0; i < 6; i++)
    {
        shadow_shader.setUniform("shadow_light_view_project", shadow_light_projection * shadow_light_view[i]);
        shadow_map_fbo[i].use();
        // glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
        // glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo.fbo_);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, depth_texture.id(), 0);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pos_texture.id(), 0);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, normal_texture.id, 0);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, flux_texture, 0);
        // GLuint tmp[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        // glDrawBuffers(3, tmp);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene->draw(shadow_shader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapper::attach(Shader &shader)
{
    shader.setUniform("shadow_limit", shadow_limit);
    // glActiveTexture(GL_TEXTURE31);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, depth_texture);
    // shader.setUniformi("shadow_map", 31);
    // glActiveTexture(GL_TEXTURE30);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, pos_texture);
    // shader.setUniformi("shadow_map_pos", 30);
    // glActiveTexture(GL_TEXTURE29);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, normal_texture);
    // shader.setUniformi("shadow_map_normal", 29);
    // glActiveTexture(GL_TEXTURE28);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, flux_texture);
    // shader.setUniformi("shadow_map_flux", 28);
    shader.setTexture("shadow_map", &depth_texture);
    shader.setTexture("shadow_map_pos", &pos_texture);
    shader.setTexture("shadow_map_normal", &normal_texture);
    shader.setTexture("shadow_map_flux", &flux_texture);
}