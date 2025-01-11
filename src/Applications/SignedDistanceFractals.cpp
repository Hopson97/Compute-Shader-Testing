#include "SignedDistanceFractals.h"

#include "../Util/Maths.h"

#include <imgui.h>

bool SignedDistanceFractals::on_init(sf::Window& window)
{

    window_ = &window;

    if (!cube_compute.load_stage("assets/shaders/SignedDistanceFractals.glsl", ShaderType::Compute) ||
        !cube_compute.link_shaders())
    {
        return false;
    }

    // Set up the compute shader output texture
    screen_texture_.create(window.getSize().x, window.getSize().y, 1, TEXTURE_PARAMS_NEAREST,
                           TextureFormat::RGBA32F);

    return true;
}

void SignedDistanceFractals::on_render(sf::Window& window)
{
    // Update the camera from keyboard/ mouse
    static sf::Clock clock;

    // Run the compute shader to create a texture
    glDisable(GL_DEPTH_TEST);
    cube_compute.bind();
    cube_compute.set_uniform("time", clock_.getElapsedTime().asSeconds());
    cube_compute.set_uniform("kind", sdf_kind_);
    cube_compute.set_uniform("distortion", sdf_distortion_);
    cube_compute.set_uniform("movement_speed", sdf_camera_speed_);
    cube_compute.set_uniform("palette_config", sdf_colours_);
    glBindImageTexture(0, screen_texture_.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute(ceil(window.getSize().x / 8), ceil(window.getSize().y / 4), 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // Render the computed output to a screen-wide quad
    screen_shader_.bind();
    screen_texture_.bind(0);
    screen_vao_.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // IMGUI
    if (ImGui::Begin("SDF Config"))
    {
        ImGui::Text("Select SDF");
        ImGui::RadioButton("Torus", &sdf_kind_, 0);
        ImGui::RadioButton("Cube", &sdf_kind_, 1);
        ImGui::RadioButton("Fractal 1", &sdf_kind_, 2);
        ImGui::RadioButton("Fractal 2", &sdf_kind_, 3);
        ImGui::RadioButton("Fractal 3", &sdf_kind_, 5);
        ImGui::RadioButton("Fractal 4", &sdf_kind_, 6);
        ImGui::RadioButton("Fractal 5", &sdf_kind_, 7);
        ImGui::RadioButton("Fractal 6", &sdf_kind_, 9);
        ImGui::RadioButton("Fractal 7", &sdf_kind_, 10);
        ImGui::RadioButton("Fractal 8", &sdf_kind_, 11);
        ImGui::RadioButton("Fractal 9", &sdf_kind_, 12);
        ImGui::RadioButton("Fractal 10", &sdf_kind_, 13);
        ImGui::RadioButton("Fractal 11", &sdf_kind_, 14);

        ImGui::RadioButton("Fractal Mix 1 + 2", &sdf_kind_, 4);
        ImGui::RadioButton("Fractal Mix 1 + 4", &sdf_kind_, 8);

        ImGui::Separator();
        ImGui::Text("Select Colour Palette");
        ImGui::RadioButton("1", &sdf_colours_, 0);
        ImGui::SameLine();
        ImGui::RadioButton("2", &sdf_colours_, 1);
        ImGui::SameLine();
        ImGui::RadioButton("3", &sdf_colours_, 2);
        ImGui::SameLine();
        ImGui::RadioButton("4", &sdf_colours_, 3);

        ImGui::Separator();


        ImGui::Text("Parameters");
        ImGui::SliderFloat("Distortion", &sdf_distortion_, 0.1f, 1.0f);
        ImGui::SliderFloat("Speed", &sdf_camera_speed_, 0.1f, 1.0f);
    }
    ImGui::End();
}
