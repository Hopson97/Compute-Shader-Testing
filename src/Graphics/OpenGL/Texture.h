#pragma once

#include <filesystem>
#include <string_view>
#include <unordered_map>

#include <SFML/Graphics/Image.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>

enum class TextureInternalFormat
{
    RGB = GL_RGB,
    RGBA = GL_RGBA,
};

enum class TextureFormat
{
    RGB8 = GL_RGB8,
    RGBA8 = GL_RGBA8,
    RGBA16F = GL_RGBA16F,
    RGBA32F = GL_RGBA32F
};

enum class TextureMinFilter
{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
};

enum class TextureMagFilter
{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
};

enum class TextureCompareFunction
{
    LessThanOrEqual = GL_LEQUAL,
    GreaterThanOrEqual = GL_GEQUAL,
    LessThan = GL_LESS,
    GreaterThan = GL_GREATER,
    Equal = GL_EQUAL,
    NotEqual = GL_NOTEQUAL,
    Always = GL_ALWAYS,
    Never = GL_NEVER,
};

enum class TextureCompareMode
{
    CompareReferenceToTexture = GL_COMPARE_REF_TO_TEXTURE,
    None = GL_NONE
};

enum class TextureWrap
{
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    Repeat = GL_REPEAT,
    MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE,
};

struct TextureParameters
{
    TextureMinFilter min_filter = TextureMinFilter::Linear;
    TextureMagFilter mag_filter = TextureMagFilter::Linear;
    TextureWrap wrap_s = TextureWrap::ClampToEdge;
    TextureWrap wrap_t = TextureWrap::ClampToEdge;
};

// clang-format off
constexpr TextureParameters TEXTURE_PARAMS_MIPMAP = {.min_filter = TextureMinFilter::LinearMipmapLinear,
                                                     .mag_filter = TextureMagFilter::Linear,
                                                     .wrap_s = TextureWrap::Repeat,
                                                     .wrap_t = TextureWrap::Repeat};

constexpr TextureParameters TEXTURE_PARAMS_NEAREST = {.min_filter = TextureMinFilter::Nearest,
                                                     .mag_filter = TextureMagFilter::Nearest,
                                                     .wrap_s = TextureWrap::Repeat,
                                                     .wrap_t = TextureWrap::Repeat};
struct GLTextureResource
{
    GLuint id = 0;

    GLTextureResource(GLenum target) { glCreateTextures(target, 1, &id); }   
    virtual ~GLTextureResource() { if(id != 0) glDeleteTextures(1, &id); }

    GLTextureResource           (const GLTextureResource& other) = delete;  
    GLTextureResource& operator=(const GLTextureResource& other) = delete;  

    GLTextureResource& operator=(GLTextureResource&& other) noexcept { id = other.id;  other.id = 0; return *this; }   
    GLTextureResource (GLTextureResource&& other) noexcept : id  (other.id){ other.id = 0; }   

    void bind(GLuint unit) const { assert(id); glBindTextureUnit(unit, id); }
    // clang-format on

    void set_filters(const TextureParameters& filters);
    void set_min_filter(TextureMinFilter filter);
    void set_mag_filter(TextureMagFilter filter);
    void set_wrap_s(TextureWrap wrap);
    void set_wrap_t(TextureWrap wrap);
    void set_compare_function(TextureCompareFunction function);
    void set_compare_mode(TextureCompareMode mode);
};

struct Texture2D : public GLTextureResource
{
    Texture2D();

    GLuint create(GLsizei width, GLsizei height, GLsizei levels = 1, TextureParameters filters = {},
                  TextureFormat format = TextureFormat::RGB8);

    GLuint create_depth_texture(GLsizei width, GLsizei height);

    [[nodiscard]] bool
    load_from_memory(const void* data, GLsizei width, GLsizei height, GLsizei levels,
                     TextureParameters filters = TEXTURE_PARAMS_MIPMAP,
                     TextureInternalFormat internal_format = TextureInternalFormat::RGBA,
                     TextureFormat format = TextureFormat::RGBA8);

    [[nodiscard]] bool load_from_image(const sf::Image& image, GLsizei levels,
                                       TextureParameters filters = TEXTURE_PARAMS_MIPMAP,
                                       TextureFormat format = TextureFormat::RGBA8);

    [[nodiscard]] bool load_from_file(const std::filesystem::path& path, GLsizei levels,
                                      bool flip_vertically, bool flip_horizontally,
                                      TextureParameters filters = TEXTURE_PARAMS_MIPMAP,
                                      TextureFormat format = TextureFormat::RGBA8);

    [[nodiscard]] bool is_loaded() const;

  private:
    bool is_loaded_ = false;
};

struct CubeMapTexture : public GLTextureResource
{
    CubeMapTexture();

    /**
        Assumes textures are called the following in the given folder:
        1 RIGHT
        2 LEFT
        3 TOP
        4 BOTTOM
        5 FRONT
        6 BACK
    */
    bool load_from_folder(const std::filesystem::path& folder);

    bool is_loaded() const;

  private:
    bool is_loaded_ = false;
};