#include "framebuffer_object.h"

FramebufferObject::FramebufferObject(const std::vector<Texture2D *> &attachments, int width, int height)
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

void FramebufferObject::use()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fb_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}