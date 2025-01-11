#pragma once

#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

enum class ShaderType
{
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    TesselationEvaluation = GL_TESS_EVALUATION_SHADER,
    TesselationControl = GL_TESS_CONTROL_SHADER,
    Compute = GL_COMPUTE_SHADER,
};

class Shader
{
  public:
    Shader() = default;
    Shader(Shader&& other) noexcept = delete;
    Shader(const Shader& other) = delete;
    Shader& operator=(Shader&& other) noexcept = delete;
    Shader& operator=(const Shader& other) = delete;
    ~Shader();

    [[nodiscard]] bool load_stage(const std::filesystem::path& file_path, ShaderType shader_type);
    [[nodiscard]] bool link_shaders();

    void bind() const;

    void set_uniform(const std::string& name, int value);
    void set_uniform(const std::string& name, float value);
    void set_uniform(const std::string& name, const glm::vec2& vector);
    void set_uniform(const std::string& name, const glm::vec3& vector);
    void set_uniform(const std::string& name, const glm::vec4& vector);
    void set_uniform(const std::string& name, const glm::mat4& matrix);

    void bind_uniform_block_index(const std::string& name, GLuint index);

  private:
    GLint get_uniform_location(const std::string& name);

  private:
    std::unordered_map<std::string, GLint> uniform_locations_;
    std::vector<GLuint> stages_;
    GLuint program_ = 0;
};
