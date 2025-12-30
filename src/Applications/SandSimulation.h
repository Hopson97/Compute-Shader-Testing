#pragma once

#include "Application.h"

class SandSimulation final : public Application
{
  public:
    void on_event(sf::Event event) override;
    bool on_init(sf::Window& window) override;
    void on_render(sf::Window& window) override;

  private:
    Shader sand_compute_;
    Texture2D screen_texture_;
    Texture2D screen_texture2_;

    BufferObject claims_buffer_;

    int generation_ = 0;
};