#include "framebuffer_object.h"

FramebufferObject::FramebufferObject(const std::vector<Texture2D *> &attachments, int width, int height, const Texture2D *depth_attachments) : width_(width), height_(height)
{
    glGenFramebuffers(1, &fb_);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_);

    for (int i = 0; i < attachments.size(); i++)
    {
        atts.push_back(GL_COLOR_ATTACHMENT0 + i);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments[i]->id(), 0);
    }
    glDrawBuffers(attachments.size(), atts.data());

    if (depth_attachments == nullptr)
    {
        glGenRenderbuffers(1, &rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_);
    }
    else
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachments->id(), 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FramebufferObject::FramebufferObject(const std::vector<TextureCube *> &attachments, int width, int height, int cube_idx, const TextureCube *depth_attachments) : width_(width), height_(height)
{
    glGenFramebuffers(1, &fb_);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_);


    for (int i = 0; i < attachments.size(); i++)
    {
        atts.push_back(GL_COLOR_ATTACHMENT0 + i);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + cube_idx, attachments[i]->id(), 0);
    }
    glDrawBuffers(attachments.size(), atts.data());

    if (depth_attachments == nullptr)
    {
        glGenRenderbuffers(1, &rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_);
    }
    else
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + cube_idx, depth_attachments->id(), 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferObject::use()
{
    glViewport(0, 0, width_, height_);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_);
    glDrawBuffers(atts.size(), atts.data());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}