#pragma once

#include <array>
#include <vector>

#include "GLResource.h"
#include "BufferObject.h"


struct VertexArray : public GLResource<glCreateVertexArrays, glDeleteVertexArrays>
{
    VertexArray() = default;
    void bind() const;
    void add_attribute(const BufferObject& vbo, GLsizei stride, GLint size, GLenum type,
                       GLuint offset);
    void reset() override;

  private:
    GLuint attribs_ = 0;
};