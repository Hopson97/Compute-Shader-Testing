#include "VertexArray.h"

// VertexArray::VertexArray(const BasicMesh& mesh)
// {
//     buffer_mesh(mesh);
// }

void VertexArray::bind() const
{
    assert(id);
    glBindVertexArray(id);
}

void VertexArray::add_attribute(const BufferObject& vbo, GLsizei stride, GLint size,
                                GLenum type, GLuint offset)
{
    glEnableVertexArrayAttrib(id, attribs_);
    glVertexArrayVertexBuffer(id, attribs_, vbo.id, 0, stride);
    glVertexArrayAttribFormat(id, attribs_, size, type, GL_FALSE, offset);
    glVertexArrayAttribBinding(id, attribs_, 0);
    attribs_++;
}

void VertexArray::reset()
{
    GLResource::destroy();
    GLResource::create();
    attribs_ = 0;
}

