#include "CloudyCompute.h"

#include <SFML/Window/Mouse.hpp>

#include "../GUI.h"
#include "../Graphics/OpenGL/GLUtils.h"
#include "../Util/Maths.h"

bool CloudyCompute::on_init(sf::Window& window)
{
    window_ = &window;
    camera_.init(window.getSize().x, window.getSize().y, 90);

    if (!cloud_compute_shader_.load_stage("assets/shaders/CloudyCompute.glsl",
                                          ShaderType::Compute) ||
        !cloud_compute_shader_.link_shaders())
    {
        return false;
    }

    if (!scene_shader_.load_stage("assets/shaders/SceneVertex.glsl", ShaderType::Vertex) ||
        !scene_shader_.load_stage("assets/shaders/SceneFragment.glsl", ShaderType::Fragment) ||
        !scene_shader_.link_shaders())
    {
        return false;
    }

    // Set up the compute shader output texture
    screen_texture_.create(window.getSize().x, window.getSize().y, 1, TEXTURE_PARAMS_NEAREST,
                           TextureFormat::RGBA32F);

    noise_texture_.load_from_file("assets/textures/rgba-noise-medium.png", 1, false, false,
                                  {
                                      .min_filter = TextureMinFilter::Linear,
                                      .mag_filter = TextureMagFilter::Linear,
                                      .wrap_s = TextureWrap::Repeat,
                                      .wrap_t = TextureWrap::Repeat,
                                  },
                                  TextureFormat::RGBA32F);

    return true;
}

void CloudyCompute::on_update(const sf::Window& window, const Keyboard& keyboard, sf::Time dt)
{
    camera_.free_controller_input(keyboard, dt, window, mouse_locked_);
    camera_.update();
}

void CloudyCompute::on_render(sf::Window& window)
{
    // Run the compute shader to create a texture
    gl::disable(gl::Capability::DepthTest);

    cloud_compute_shader_.bind();
    cloud_compute_shader_.set_uniform("inv_projection",
                                      glm::inverse(camera_.get_projection_matrix()));
    cloud_compute_shader_.set_uniform("inv_view", glm::inverse(camera_.get_view_matrix()));
    cloud_compute_shader_.set_uniform("position", camera_.transform.position);
    cloud_compute_shader_.set_uniform("time", clock_.getElapsedTime().asSeconds());

    glBindImageTexture(0, screen_texture_.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, noise_texture_.id, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    noise_texture_.bind(1);
    gl::dispatch_compute(std::ceil(window.getSize().x / 8), std::ceil(window.getSize().y / 4), 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // Render the computed output to a screen-wide quad
    screen_shader_.bind();
    screen_texture_.bind(0);
    screen_vao_.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Render the grid
    glBindTexture(GL_TEXTURE_2D, 0);
    gl::enable(gl::Capability::DepthTest);
    scene_shader_.bind();
    scene_shader_.set_uniform("projection_matrix", camera_.get_projection_matrix());
    scene_shader_.set_uniform("view_matrix", camera_.get_view_matrix());
    scene_shader_.set_uniform("model_matrix", create_model_matrix({.position = {0, -1, 0}}));
    grid_mesh_.bind();
    grid_mesh_.draw(GL_LINES);

    GUI::debug_window(camera_.transform.position, camera_.transform.rotation);
}

void CloudyCompute::on_event(sf::Event event)
{
    if (auto key = event.getIf<sf::Event::KeyReleased>())
    {
        if (key->code == sf::Keyboard::Key::L)
        {
            mouse_locked_ = !mouse_locked_;
        }
    }
}