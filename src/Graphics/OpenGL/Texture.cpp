#include "Texture.h"

#include <SFML/Graphics/Image.hpp>
#include <array>
#include <iostream>

//=======================
// == Helper functions ==
//=======================
namespace
{
    const std::array<std::string, 6> CUBE_TEXTURE_NAMES = {"right.png",  "left.png", "top.png",
                                                           "bottom.png", "back.png", "front.png"};

    [[nodiscard]] bool load_image_from_file(const std::filesystem::path& path, bool flip_vertically,
                                            bool flip_horizontally, sf::Image& out_image)
    {
        if (!out_image.loadFromFile(path.string()))
        {
            return false;
        }

        if (flip_vertically)
        {
            out_image.flipVertically();
        }
        if (flip_horizontally)
        {
            out_image.flipHorizontally();
        }

        return true;
    }
} // namespace

//=======================================
// == GLTextureResource Implementation ==
//=======================================
void GLTextureResource::set_filters(const TextureParameters& filters)
{
    assert(id != 0);
    set_min_filter(filters.min_filter);
    set_mag_filter(filters.mag_filter);
    set_wrap_s(filters.wrap_s);
    set_wrap_t(filters.wrap_t);
}

void GLTextureResource::set_min_filter(TextureMinFilter filter)
{
    assert(id != 0);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(filter));
}

void GLTextureResource::set_mag_filter(TextureMagFilter filter)
{
    assert(id != 0);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(filter));
}

void GLTextureResource::set_wrap_s(TextureWrap wrap)
{
    assert(id != 0);
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrap));
}

void GLTextureResource::set_wrap_t(TextureWrap wrap)
{
    assert(id != 0);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrap));
}

void GLTextureResource::set_compare_function(TextureCompareFunction function)
{
    assert(id != 0);
    glTextureParameteri(id, GL_TEXTURE_COMPARE_FUNC, static_cast<GLenum>(function));
}

void GLTextureResource::set_compare_mode(TextureCompareMode mode)
{
    assert(id != 0);
    glTextureParameteri(id, GL_TEXTURE_COMPARE_MODE, static_cast<GLenum>(mode));
}

//================================
// == Texture2D Implementation ===
//================================
Texture2D::Texture2D()
    : GLTextureResource(GL_TEXTURE_2D)
{
}

GLuint Texture2D::create(GLsizei width, GLsizei height, GLsizei levels, TextureParameters filters,
                         TextureFormat format)
{
    glTextureStorage2D(id, levels, static_cast<GLenum>(format), width, height);

    set_filters(filters);

    return id;
}

GLuint Texture2D::create_depth_texture(GLsizei width, GLsizei height)
{
    // As there is not DSA equivalent for glTexImage2D, traditional binding must be used to create
    // a texture with GL_DEPTH_COMPONENT - hence binding to unit 0 before creating the texture
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr);
    set_filters({});
    set_compare_function(TextureCompareFunction::LessThanOrEqual);
    set_compare_mode(TextureCompareMode::CompareReferenceToTexture);

    return id;
}

bool Texture2D::load_from_memory(const void* data, GLsizei width, GLsizei height, GLsizei levels,
                                 TextureParameters filters, TextureInternalFormat internal_format,
                                 TextureFormat format)
{

    // Allocate the storage
    glTextureStorage2D(id, levels, static_cast<GLenum>(format), width, height);

    // Upload the data
    glTextureSubImage2D(id, 0, 0, 0, width, height, static_cast<GLenum>(internal_format),
                        GL_UNSIGNED_BYTE, data);

    if (filters.min_filter == TextureMinFilter::LinearMipmapLinear ||
        filters.min_filter == TextureMinFilter::LinearMipmapNearest)
    {
        glGenerateTextureMipmap(id);
    }

    set_filters(filters);
    is_loaded_ = true;
    return true;
}

bool Texture2D::load_from_image(const sf::Image& image, GLsizei levels, TextureParameters filters,
                                TextureFormat format)
{
    const auto w = image.getSize().x;
    const auto h = image.getSize().y;
    const auto data = image.getPixelsPtr();

    return load_from_memory(data, w, h, levels, filters, TextureInternalFormat::RGBA, format);
}

bool Texture2D::load_from_file(const std::filesystem::path& path, GLsizei levels,
                               bool flip_vertically, bool flip_horizontally,
                               TextureParameters filters, TextureFormat format)
{
    sf::Image image;
    if (!load_image_from_file(path, flip_vertically, flip_horizontally, image))
    {
        return false;
    }
    return load_from_image(image, levels, filters, format);
}

bool Texture2D::is_loaded() const
{
    return is_loaded_;
}

//======================================
// == Cubemap Texture Implementation ===
//======================================
CubeMapTexture::CubeMapTexture()
    : GLTextureResource(GL_TEXTURE_CUBE_MAP)
{
}

bool CubeMapTexture::load_from_folder(const std::filesystem::path& folder)
{
    // Assumes textures are called the following in the given folder:
    // 1 RIGHT
    // 2 LEFT
    // 3 TOP
    // 4 BOTTOM
    // 5 FRONT
    // 6 BACK

    bool created = false;

    for (int i = 0; i < 6; i++)
    {
        sf::Image image;
        if (!load_image_from_file(folder / CUBE_TEXTURE_NAMES[i], false, false, image))
        {
            return false;
        }

        const auto w = image.getSize().x;
        const auto h = image.getSize().y;
        const auto data = image.getPixelsPtr();

        if (!created)
        {
            glTextureStorage2D(id, 1, GL_RGBA8, w, h);
            created = true;
        }

        glTextureSubImage3D(id, 0, 0, 0, i, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    set_filters({});
    return created;
}

bool CubeMapTexture::is_loaded() const
{
    return false;
}
