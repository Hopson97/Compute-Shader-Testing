#pragma once

#include <array>
#include <span>
#include <vector>

#include "GLResource.h"
#include "Shader.h"

enum class BindBufferTarget
{
    AtomicCounterBuffer = GL_ATOMIC_COUNTER_BUFFER,
    TransformFeedbackBuffer = GL_TRANSFORM_FEEDBACK_BUFFER,
    UniformBuffer = GL_UNIFORM_BUFFER,
    ShaderStorageBuffer = GL_SHADER_STORAGE_BUFFER,
};

struct BufferObject : public GLResource<glCreateBuffers, glDeleteBuffers>
{
    template <typename T>
    void buffer_data(const std::vector<T>& data)
    {
        glNamedBufferStorage(id, sizeof(data[0]) * data.size(), data.data(),
                             GL_DYNAMIC_STORAGE_BIT);
    }

    template <typename T>
    void buffer_data(const T& data)
    {
        glNamedBufferStorage(id, sizeof(data), data, GL_DYNAMIC_STORAGE_BIT);
    }

    template <typename T>
    void buffer_sub_data(GLintptr offset, const T& data)
    {
        glNamedBufferSubData(id, offset, sizeof(data), &data);
    }

    template <typename T, int N>
    void buffer_sub_data(GLintptr offset, const std::array<T, N>& data)
    {
        glNamedBufferSubData(id, offset, sizeof(data[0]) * data.size(), data.data());
    }

    template <typename T>
    void buffer_sub_data(GLintptr offset, const std::vector<T> data)
    {
        glNamedBufferSubData(id, offset, sizeof(data[0]) * data.size(), data.data());
    }

    void create_store(GLsizeiptr size)
    {
        glNamedBufferStorage(id, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

    void bind_buffer_base(BindBufferTarget target, GLuint index)
    {
        glBindBufferBase(static_cast<GLenum>(target), index, id);
    }

    void bind_buffer_range(BindBufferTarget target, GLuint index, GLsizeiptr bytes)
    {
        // @TODO is this ever anything other than 0?
        glBindBufferRange(static_cast<GLenum>(target), index, id, 0, bytes);
    }

    template <typename T>
    void create_as_ubo(int index, int count = 1)
    {
        create_as_shader_binding<T, BindBufferTarget::UniformBuffer>(index, count);
    }

    template <typename T, int Count = 1>
    void create_as_ssbo(int index, int count = 1)
    {
        create_as_shader_binding<T, BindBufferTarget::ShaderStorageBuffer>(index, count);
    }

  private:
    template <typename T, BindBufferTarget Target>
    void create_as_shader_binding(int index, int count = 1)
    {
        create_store(sizeof(T) * count);
        bind_buffer_base(Target, index);
        bind_buffer_range(Target, index, sizeof(T) * count);
    }
};