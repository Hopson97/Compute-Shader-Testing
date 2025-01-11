#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Graphics/OpenGL/Shader.h"
#include "../Graphics/OpenGL/Texture.h"
#include "../Graphics/OpenGL/VertexArray.h"

class Keyboard;

class Application
{
  public:
    virtual ~Application() = default;

    [[nodiscard]] bool init(sf::Window& window);

    virtual void on_event([[maybe_unused]] sf::Event event)
    {
    }

    virtual void on_update(const sf::Window& window, const Keyboard& keyboard, sf::Time dt)
    {
    }

    virtual void on_render(sf::Window& window) = 0;

  protected:
    virtual bool on_init(sf::Window& window) = 0;

    Shader screen_shader_;
    VertexArray screen_vao_;
};
