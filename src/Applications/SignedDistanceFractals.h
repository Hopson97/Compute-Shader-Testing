#pragma once


#include "Application.h"

class SignedDistanceFractals : public Application
{
  public:
    bool on_init(sf::Window& window) override;
    void on_render(sf::Window& window) override;

  private:
    sf::Window* window_ = nullptr;

    Shader cube_compute;
    Texture2D screen_texture_;

    sf::Clock clock_;

    // Config
    int sdf_kind_ = 2;
    int sdf_colours_ = 0;
    float sdf_distortion_ = 0.25f;
    float sdf_camera_speed_ = 0.1f;
};