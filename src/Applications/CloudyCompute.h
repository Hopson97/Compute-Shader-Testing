#pragma once

#include <SFML/System/Clock.hpp>

#include "../Graphics/Camera.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/OpenGL/BufferObject.h"
#include "Application.h"

class CloudyCompute final : public Application
{
  public:
    bool on_init(sf::Window& window) override;
    void on_event(sf::Event event) override;
    void on_update(const sf::Window& window, const Keyboard& keyboard, sf::Time dt) override;
    void on_render(sf::Window& window) override;

  private:
    Shader cloud_compute_shader_;

    Shader scene_shader_;
    Texture2D screen_texture_;
    Texture2D noise_texture_;

    PerspectiveCamera camera_;

    Mesh grid_mesh_ = generate_grid_mesh(10);

    sf::Window* window_ = nullptr;

    bool mouse_locked_ = false;

    sf::Clock clock_;
};

