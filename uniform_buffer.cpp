#include "uniform_buffer.h"

UniformBuffer::UniformBuffer()
{
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
}

void UniformBuffer::use(int id)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, id, ubo);
}

void UniformBuffer::setData(int size, const void *value, GLuint mode)
{
    glBufferData(GL_UNIFORM_BUFFER, size, value, mode);
}