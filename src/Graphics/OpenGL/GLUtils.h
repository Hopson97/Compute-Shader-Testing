#pragma once

#include <glad/glad.h>

namespace GL
{
    enum class Face
    {
        Back = GL_BACK,
        Front = GL_FRONT,
        FrontAndBack = GL_FRONT_AND_BACK,
    };

    enum class Capability
    {
        Blend = GL_BLEND,
        DepthTest = GL_DEPTH_TEST,
        CullFace = GL_CULL_FACE,
        ScissorTest = GL_SCISSOR_TEST,
        StencilTest = GL_STENCIL_TEST
    };

    enum class PolygonMode
    {
        Point = GL_POINT,
        Line = GL_LINE,
        Fill = GL_FILL
    };

    void enable_debugging();

    void cull_face(Face face);

    void enable(Capability capability);
    void disable(Capability capability);

    void polygon_mode(Face face, PolygonMode mode);

    // This is just to avoid compiler warnings
    template <typename Tx, typename Ty, typename Tz>
    void dispatch_compute(Tx x, Ty y, Tz z)
    {
        glDispatchCompute(static_cast<GLuint>(x), static_cast<GLuint>(y), static_cast<GLuint>(z));
    }

} // namespace GL
