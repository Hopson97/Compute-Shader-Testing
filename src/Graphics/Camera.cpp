#include "Camera.h"

#include <imgui.h>

#include "../Util/Keyboard.h"

PerspectiveCamera::PerspectiveCamera(unsigned width, unsigned height, float fov)
    : aspect_{static_cast<float>(width) / static_cast<float>(height)}
    , fov_{fov}
{
    projection_matrix_ = glm::perspective(glm::radians(fov_), aspect_, near_, far_);
}

void PerspectiveCamera::init(unsigned width, unsigned height, float fov)
{
    aspect_ = static_cast<float>(width) / static_cast<float>(height);
    fov_ = fov;
    projection_matrix_ = glm::perspective(glm::radians(fov_), aspect_, near_, far_);
}

void PerspectiveCamera::update()
{
    forwards_ = forward_vector(transform.rotation);
    glm::vec3 centre = transform.position + glm::normalize(forwards_);
    view_matrix_ = glm::lookAt(transform.position, centre, {0, 1, 0});
}

void PerspectiveCamera::gui()
{
    if (ImGui::Begin("Camera")) {
        if (ImGui::SliderFloat("FOV", &fov_, 1.0f, 179.0f))
        {
            projection_matrix_ = glm::perspective(glm::radians(fov_), aspect_, near_, far_);
        }
        if (ImGui::SliderFloat("Near", &near_, 1.0f, 10.0f))
        {
            projection_matrix_ = glm::perspective(glm::radians(fov_), aspect_, near_, far_);
        }
        if (ImGui::SliderFloat("Far", &far_, 100.0f, 10000.0f))
        {
            projection_matrix_ = glm::perspective(glm::radians(fov_), aspect_, near_, far_);
        }
    }
    ImGui::End();
}

const glm::mat4& PerspectiveCamera::get_view_matrix() const
{
    return view_matrix_;
}

const glm::mat4& PerspectiveCamera::get_projection_matrix() const
{
    return projection_matrix_;
}

const glm::vec3& PerspectiveCamera::get_forwards() const
{
    return forwards_;
}

void PerspectiveCamera::free_controller_input(const Keyboard& keyboard, sf::Time dt,
                                               const sf::Window& window, bool is_rotation_locked)
{
    if (!window.hasFocus())
    {
        return;
    }

    // Keyboard Input
    glm::vec3 move{0.0f};
    if (keyboard.is_key_down(sf::Keyboard::Key::W))
    {
        move += forward_vector(transform.rotation);
    }
    else if (keyboard.is_key_down(sf::Keyboard::Key::S))
    {
        move += backward_vector(transform.rotation);
    }

    if (keyboard.is_key_down(sf::Keyboard::Key::A))
    {
        move += left_vector(transform.rotation);
    }
    else if (keyboard.is_key_down(sf::Keyboard::Key::D))
    {
        move += right_vector(transform.rotation);
    }

    if (keyboard.is_key_down(sf::Keyboard::Key::LShift))
    {
        move *= 10.0f;
    }
    transform.position += move * dt.asSeconds();

    if (!is_rotation_locked)
    {
        static auto last_mouse = sf::Mouse::getPosition(window);
        auto change = sf::Mouse::getPosition(window) - last_mouse;
        auto& r = transform.rotation;

        r.x -= static_cast<float>(change.y * 0.35);
        r.y += static_cast<float>(change.x * 0.35);

        sf::Mouse::setPosition({(int)window.getSize().x / 2, (int)window.getSize().y / 2}, window);
        last_mouse = sf::Mouse::getPosition(window);

        r.x = glm::clamp(r.x, -89.9f, 89.9f);
        if (r.y >= 360.0f)
        {
            r.y = 0.0f;
        }
        else if (r.y < 0.0f)
        {
            r.y = 359.9f;
        }
    }
}

