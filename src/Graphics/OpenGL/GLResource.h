#pragma once

#include <cassert>
#include <iostream>

#include <glad/glad.h>

// clang-format off
/**
 * @brief Wrapper class for any OpenGL object that has a standard glCreate* or glDelete* function
 * for construction and destruction
 * 
 * @tparam CreateFunction Function to create an OpenGL object (eg &glCreateVertexArrays)
 * @tparam DeleteFunction Function to delete an OpenGL object (eg &glDeleteVertexArrays) 
 */
template<auto& CreateFunction, auto& DeleteFunction>
struct GLResource
{
    GLuint id = 0;

    GLResource          () { create();  }
    virtual ~GLResource () { destroy(); }

    GLResource(const GLResource& other)             = delete;   
    GLResource& operator=(const GLResource& other)  = delete;   

    GLResource& operator=(GLResource&& other) noexcept  { destroy(); id = other.id;  other.id = 0; return *this; }   
    GLResource(GLResource&& other) noexcept  : id  (other.id){ other.id = 0; }

    virtual void reset() { destroy(); create(); }

    void create()   { if(id == 0) {(*CreateFunction)(1, &id);           } }
    void destroy()  { if(id != 0) {(*DeleteFunction)(1, &id); id = 0;   } }
};
// clang-format on
