#pragma once

#include "../Util/Maths.h"
#include <SFML/Window/Window.hpp>

class Keyboard;

struct PerspectiveCamera
{
  public:
    Transform transform;

  public:
    PerspectiveCamera() = default;
    PerspectiveCamera(unsigned window_size, unsigned window_width, float fov);
    void init(unsigned window_size, unsigned window_width, float fov);
    void update();
    void gui();

    const glm::mat4& get_view_matrix() const;
    const glm::mat4& get_projection_matrix() const;
    const glm::vec3& get_forwards() const;

    void free_controller_input(const Keyboard& keyboard, sf::Time dt, const sf::Window& window,
                               bool is_rotation_locked);

  private:
    glm::mat4 projection_matrix_{1.0f};
    glm::mat4 view_matrix_{1.0f};
    glm::vec3 forwards_{0.0f};

    float aspect_ = 0;
    float fov_ = 0;
    float near_ = 0.2f;
    float far_ = 2000.0f;
};