#include "shadow_mapper.h"

ShadowMapper::ShadowMapper() : shadow_shader("../shadow.vs", "../shadow.fs")
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

void ShadowMapper::lightPass(glm::vec3 shadow_light_pos, Model *scene)
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

void ShadowMapper::attach(Shader &shader)
{
    shader.setUniform("shadowLimit", shadow_limit);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map_texture);
    shader.setUniformi("shadow_map", 15);
}