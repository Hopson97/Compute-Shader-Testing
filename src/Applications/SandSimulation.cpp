#include "SandSimulation.h"

#include "../Graphics/OpenGL/GLUtils.h"
#include <SFML/Graphics/Image.hpp>

bool SandSimulation::on_init(sf::Window& window)
{
    if (!sand_compute_.load_stage("assets/shaders/SandSimulationCompute.glsl",
                                  ShaderType::Compute) ||
        !sand_compute_.link_shaders())
    {
        return false;
    }

    auto width = window.getSize().x;
    auto height = window.getSize().y;

    sf::Image image(window.getSize());
    if (!screen_texture_.load_from_image(image, 1, TEXTURE_PARAMS_NEAREST, TextureFormat::RGBA32F))
    {
        return false;
    }

    screen_texture2_.create(width, height, 1, TEXTURE_PARAMS_NEAREST, TextureFormat::RGBA32F);

    claims_buffer_.create_as_ssbo<GLuint>(0, width * height);

    sand_compute_.set_uniform("brush_size", 5.0f);

    return true;
}

void SandSimulation::on_event(sf::Event event)
{
    if (event.is<sf::Event::MouseButtonPressed>())
    {
        sand_compute_.set_uniform("mouse_down", true);
    }
    if (event.is<sf::Event::MouseButtonReleased>())
    {
        sand_compute_.set_uniform("mouse_down", false);
    }
}

void SandSimulation::on_render(sf::Window& window)
{
    sand_compute_.bind();

    // Clear the claims buffer
    glClearNamedBufferData(claims_buffer_.id, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

    if (generation_ % 2 == 0)
    {
        glBindImageTexture(0, screen_texture_.id, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, screen_texture2_.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        screen_texture2_.bind(0);
    }
    else
    {
        glBindImageTexture(0, screen_texture2_.id, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, screen_texture_.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        screen_texture_.bind(0);
    }
    generation_++;
    sand_compute_.set_uniform("generation", generation_);

    auto mouse_coords = sf::Mouse::getPosition(window);
    sand_compute_.set_uniform("mouse_coords",
                              glm::ivec2{mouse_coords.x, window.getSize().y - mouse_coords.y});

    gl::dispatch_compute(ceil(window.getSize().x / 8), ceil(window.getSize().y / 4), 1u);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    screen_shader_.bind();
    screen_vao_.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}