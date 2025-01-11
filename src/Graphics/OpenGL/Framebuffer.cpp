#include "Framebuffer.h"

#include <iostream>

Framebuffer::Framebuffer(GLuint width, GLuint height)
    : width{width}
    , height{height}
{
}

Framebuffer::~Framebuffer()
{
    for (auto& renderbuffer : renderbuffers_)
    {
        glDeleteRenderbuffers(1, &renderbuffer);
    }
}

void Framebuffer::bind() const
{
    assert(id);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    // glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::bind_texture(GLuint index, GLuint unit) const
{
    assert(index < textures_.size());
    textures_[index].bind(unit);
}

GLuint Framebuffer::get_texture_id(GLuint index) const
{
    assert(index < textures_.size());
    return textures_[index].id;
}

Texture2D& Framebuffer::get_texture(GLuint index)
{
    assert(index < textures_.size());
    return textures_[index];
}

Framebuffer& Framebuffer::attach_colour(TextureFormat format, const TextureParameters& filters)
{
    assert(textures_.size() < GL_MAX_COLOR_ATTACHMENTS - 1);
    GLenum attachment = GL_COLOR_ATTACHMENT0 + static_cast<int>(textures_.size());

    Texture2D& texture = textures_.emplace_back();
    texture.create(width, height, 1, filters, format);
    glNamedFramebufferTexture(id, attachment, texture.id, 0);
    return *this;
}

Framebuffer& Framebuffer::attach_renderbuffer()
{
    GLuint& rbo = renderbuffers_.emplace_back();
    glCreateRenderbuffers(1, &rbo);
    glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, width, height);
    glNamedFramebufferRenderbuffer(id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    return *this;
}

Framebuffer& Framebuffer::attach_depth_buffer()
{
    Texture2D& texture = textures_.emplace_back();
    texture.create_depth_texture(width, height);
    glNamedFramebufferTexture(id, GL_DEPTH_ATTACHMENT, texture.id, 0);
    glNamedFramebufferDrawBuffer(id, GL_NONE);
    glNamedFramebufferReadBuffer(id, GL_NONE);
    return *this;
}

bool Framebuffer::is_complete() const
{
    if (auto status = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::println(std::cerr, "Framebuffer incomplete with status: {}.", status);
        return false;
    }
    return true;
}