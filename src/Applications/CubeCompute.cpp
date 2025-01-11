#include "CubeCompute.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "../GUI.h"
#include "../Graphics/OpenGL/GLUtils.h"
#include "../Util/Maths.h"

bool CubeCompute::on_init(sf::Window& window)
{

    window_ = &window;
    camera_.init(window.getSize().x, window.getSize().y, 90);

    if (!cube_compute.load_stage("assets/shaders/CubeCompute.glsl", ShaderType::Compute) ||
        !cube_compute.link_shaders())
    {
        return false;
    }

    if (!scene_shader_.load_stage("assets/shaders/SceneVertex.glsl", ShaderType::Vertex) ||
        !scene_shader_.load_stage("assets/shaders/SceneFragment.glsl", ShaderType::Fragment) ||
        !scene_shader_.link_shaders())
    {
        return false;
    }

    // Cube

    if (!cube_texture_.load_from_file("assets/textures/debug.png", 1, false, false))
    {
        return false;
    }
    cube_texture_.set_min_filter(TextureMinFilter::Nearest);
    cube_texture_.set_mag_filter(TextureMagFilter::Nearest);

    // Set up the compute shader output texture
    screen_texture_.create(window.getSize().x, window.getSize().y, 1, TEXTURE_PARAMS_NEAREST,
                           TextureFormat::RGBA32F);

    return true;
}

void CubeCompute::update_camera(sf::Time dt)
{
    // Keyboard Input
    glm::vec3 move{0.0f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        move += forward_vector(camera_.transform.rotation);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        move += backward_vector(camera_.transform.rotation);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        move += left_vector(camera_.transform.rotation);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        move += right_vector(camera_.transform.rotation);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        move *= 10.0f;
    }
    camera_.transform.position += move * dt.asSeconds();

    if (!mouse_locked_)
    {
        static auto last_mouse = sf::Mouse::getPosition(*window_);
        auto change = sf::Mouse::getPosition(*window_) - last_mouse;
        auto& r = camera_.transform.rotation;

        r.x -= static_cast<float>(change.y * 0.35);
        r.y += static_cast<float>(change.x * 0.35);

        sf::Mouse::setPosition({(int)window_->getSize().x / 2, (int)window_->getSize().y / 2},
                               *window_);
        last_mouse = sf::Mouse::getPosition(*window_);

        r.x = glm::clamp(r.x, -89.9f, 89.9f);
        if (r.y >= 360.0f)
            r.y = 0.0f;
        else if (r.y < 0.0f)
            r.y = 359.9f;
    }
    camera_.update();
}

void CubeCompute::on_render(sf::Window& window)
{
    // Update the camera from keyboard/ mouse
    static sf::Clock clock;
    update_camera(clock.restart());

    // Run the compute shader to create a texture
    glDisable(GL_DEPTH_TEST);
    cube_compute.bind();
    cube_compute.set_uniform("inv_projection", glm::inverse(camera_.get_projection_matrix()));
    cube_compute.set_uniform("inv_view", glm::inverse(camera_.get_view_matrix()));
    cube_compute.set_uniform("position", camera_.transform.position);

    glBindImageTexture(0, screen_texture_.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    GL::dispatch_compute(std::ceil(window.getSize().x / 8), std::ceil(window.getSize().y / 4), 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // Render the computed output to a screen-wide quad
    screen_shader_.bind();
    screen_texture_.bind(0);
    screen_vao_.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Render a regular scene
    glEnable(GL_DEPTH_TEST);
    scene_shader_.bind();
    scene_shader_.set_uniform("projection_matrix", camera_.get_projection_matrix());
    scene_shader_.set_uniform("view_matrix", camera_.get_view_matrix());
    scene_shader_.set_uniform("model_matrix", create_model_matrix({.position = {5, 0, 5}}));

    // cube_texture_.bind(0);
    cube_mesh_.bind();
    cube_mesh_.draw();

    glBindTexture(GL_TEXTURE_2D, 0);
    scene_shader_.set_uniform("model_matrix", create_model_matrix({.position = {0, -1, 0}}));
    grid_mesh_.bind();
    grid_mesh_.draw(GL_LINES);

    GUI::debug_window(camera_.transform.position, camera_.transform.rotation);
}

void CubeCompute::on_event(sf::Event event)
{
    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::L)
    {
        mouse_locked_ = !mouse_locked_;
        std::cout << "Mouse state: " << (mouse_locked_ ? "Locked" : "Unlocked") << '\n';
    }
}
