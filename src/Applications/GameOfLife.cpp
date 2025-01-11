#include "GameOfLife.h"

#include "../Graphics/OpenGL/GLUtils.h"
#include <SFML/Graphics/Image.hpp>

bool GameOfLife::on_init(sf::Window& window)
{
    if (!game_of_life_compute_.load_stage("assets/shaders/ConwayCompute.glsl",
                                          ShaderType::Compute) ||
        !game_of_life_compute_.link_shaders())
    {
        return false;
    }

    sf::Image image;
    image.create(window.getSize().x, window.getSize().y);
    for (int y = 0; y < static_cast<int>(image.getSize().y); y++)
    {
        for (int x = 0; x < static_cast<int>(image.getSize().x); x++)
        {
            image.setPixel(x, y, rand() % 50 > 20 ? sf::Color::Black : sf::Color::White);
        }
    }

    // Set up the double buffer
    if (!screen_texture_.load_from_image(image, 1, TEXTURE_PARAMS_NEAREST,
                                         TextureInternalFormat::RGBA, TextureFormat::RGBA32F))
    {
        return false;
    }

    screen_texture2_.create(window.getSize().x, window.getSize().y, 1, TEXTURE_PARAMS_NEAREST,
                            TextureFormat::RGBA32F);

    return true;
}

void GameOfLife::on_render(sf::Window& window)
{
    // The compute shader uses a double buffer to prevent read/write to same pixel by different work
    // groups
    game_of_life_compute_.bind();
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

    gl::dispatch_compute(ceil(window.getSize().x / 8), ceil(window.getSize().y / 4), 1u);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    screen_shader_.bind();
    screen_vao_.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}