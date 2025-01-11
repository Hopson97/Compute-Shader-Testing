#pragma once

#include <unordered_map>

#include "GLResource.h"
#include "Texture.h"

class Framebuffer : public GLResource<glCreateFramebuffers, glDeleteFramebuffers>
{
  public:
    Framebuffer(GLuint width, GLuint height);
    ~Framebuffer();

    void bind() const;
    void bind_texture(GLuint index, GLuint unit) const;
    GLuint get_texture_id(GLuint index) const;
    Texture2D& get_texture(GLuint index);

    Framebuffer& attach_colour(TextureFormat format, const TextureParameters& filters = {});
    Framebuffer& attach_renderbuffer();
    Framebuffer& attach_depth_buffer();

    [[nodiscard]] bool is_complete() const;

    const GLuint width = 0;
    const GLuint height = 0;

  private:
    std::vector<Texture2D> textures_;
    std::vector<GLuint> renderbuffers_;
};
