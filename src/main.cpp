#include <iostream>

#include <SFML/Window/Window.hpp>
#include <glad/glad.h>
#include <imgui.h>

#include "Applications/CubeCompute.h"
#include "Applications/GameOfLife.h"
#include "Applications/SignedDistanceFractals.h"
#include "GUI.h"
#include "Graphics/OpenGL/GLUtils.h"
#include "Util/Keyboard.h"
#include "Util/Profiler.h"
#include "Util/TimeStep.h"

namespace
{
    void handle_event(const sf::Event& e, sf::Window& window, bool& show_debug_info);
} // namespace

int main()
{
    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.stencilBits = 8;
    context_settings.antialiasingLevel = 4;
    context_settings.majorVersion = 4;
    context_settings.minorVersion = 5;
    context_settings.attributeFlags = sf::ContextSettings::Core;

    const char* TITLE = "Compute Shaders - Press F1 For Debug";
    sf::Window window{{1600, 900}, TITLE, sf::Style::Close, context_settings};


    window.setVerticalSyncEnabled(true);
    window.setActive(true);
    window.setPosition({window.getPosition().x, 10});

    if (!gladLoadGL())
    {
        std::print(std::cerr, "Failed to initialise OpenGL - Is OpenGL linked correctly?");
        return EXIT_FAILURE;
    }
    glClearColor(50.0f / 255.0f, 205.0f / 255.0f, 250.0f / 255.0f, 0);
    glViewport(0, 0, window.getSize().x, window.getSize().y);
    GL::enable_debugging();

    if (!GUI::init(&window))
    {
        std::print(std::cerr, "Failed to initialise Imgui.");
        return EXIT_FAILURE;
    }

    TimeStep updater{50};
    Profiler profiler;
    bool show_debug_info = false;

    // std::unique_ptr<Application> app = std::make_unique<GameOfLife>();
    std::unique_ptr<Application> app = std::make_unique<CubeCompute>();
    // std::unique_ptr<Application> app = std::make_unique<SignedDistanceFractals>();
    if (!app->init(window))
    {
        return -1;
    }

    Keyboard keyboard;

    // -------------------
    // ==== Main Loop ====
    // -------------------
    sf::Clock clock;
    while (window.isOpen())
    {
        GUI::begin_frame();
        for (sf::Event e{}; window.pollEvent(e);)
        {
            GUI::event(window, e);
            keyboard.update(e);
            app->on_event(e);
            handle_event(e, window, show_debug_info);
        }
        // auto dt = clock.restart();

        /*
        // Update
        {
            auto& update_profiler = profiler.begin_section("Update");
            app.on_update(keyboard, dt);
            update_profiler.end_section();
        }

        // Fixed-rate update
        {
            auto& fixed_update_profiler = profiler.begin_section("Fixed Update");
            updater.update([&](sf::Time dt) { app.on_fixed_update(dt); });
            fixed_update_profiler.end_section();
        }
        */

        // Render
        {
            auto& render_profiler = profiler.begin_section("Render");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            app->on_render(window);
            render_profiler.end_section();
        }

        // Show profiler
        profiler.end_frame();
        if (show_debug_info)
        {
            profiler.gui();
        }

        if (ImGui::Begin("Switch Application Type"))
        {
            bool update = false;
            if (ImGui::Button("Game Of Life"))
            {
                update = true;
                app = std::make_unique<GameOfLife>();
            }
            if (ImGui::Button("Cubes"))
            {
                update = true;
                app = std::make_unique<CubeCompute>();
            }
            if (ImGui::Button("SignedDistanceFields"))
            {
                update = true;
                app = std::make_unique<SignedDistanceFractals>();
            }

            if (update && !app->init(window))
            {
                break;
            }
        }
        ImGui::End();

        // --------------------------
        // ==== End Frame ====
        // --------------------------
        GUI::render();
        window.display();
    }

    // --------------------------
    // ==== Graceful Cleanup ====
    // --------------------------
    GUI::shutdown();
}

namespace
{
    void handle_event(const sf::Event& e, sf::Window& window, bool& show_debug_info)
    {
        switch (e.type)
        {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyReleased:
                switch (e.key.code)
                {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;

                    case sf::Keyboard::F1:
                        show_debug_info = !show_debug_info;
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
} // namespace