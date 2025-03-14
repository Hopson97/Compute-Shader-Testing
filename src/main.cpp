#include <iostream>

#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Window.hpp>
#include <glad/glad.h>
#include <imgui.h>

#include "Applications/CloudyCompute.h"
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
    void handle_event(const sf::Event& event, sf::Window& window, bool& show_debug_info,
                      bool& close_requested);
} // namespace

int main()
{
    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.stencilBits = 8;
    context_settings.antiAliasingLevel = 4;
    context_settings.majorVersion = 4;
    context_settings.minorVersion = 5;
    context_settings.attributeFlags = sf::ContextSettings::Debug;

    sf::Window window(sf::VideoMode({1600, 900}), "Yahvg - Press F1 For Debug", sf::Style::Close,
                      sf::State::Windowed, context_settings);

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
    gl::enable_debugging();

    if (!GUI::init(&window))
    {
        std::print(std::cerr, "Failed to initialise Imgui.");
        return EXIT_FAILURE;
    }

    TimeStep updater{50};
    Profiler profiler;
    bool show_debug_info = false;

    // std::unique_ptr<Application> app = std::make_unique<GameOfLife>();
    std::unique_ptr<Application> app = std::make_unique<CloudyCompute>();
    // std::unique_ptr<Application> app = std::make_unique<CubeCompute>();
    //  std::unique_ptr<Application> app = std::make_unique<SignedDistanceFractals>();
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
        bool close_requested = false;
        while (auto event = window.pollEvent())
        {
            GUI::event(window, *event);
            keyboard.update(*event);
            app->on_event(*event);
            handle_event(*event, window, show_debug_info, close_requested);
        }
        auto dt = clock.restart();

        // Update
        {
            auto& update_profiler = profiler.begin_section("Update");
            app->on_update(window, keyboard, dt);
            update_profiler.end_section();
        }
        /*
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
            if (ImGui::Button("ClodyCompute"))
            {
                update = true;
                app = std::make_unique<CloudyCompute>();
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
        if (close_requested)
        {
            window.close();
        }
    }

    // --------------------------
    // ==== Graceful Cleanup ====
    // --------------------------
    GUI::shutdown();
}

namespace
{
    void handle_event(const sf::Event& event, sf::Window& window, bool& show_debug_info,
                      bool& close_requested)
    {
        if (event.is<sf::Event::Closed>())
        {
            close_requested = true;
        }
        else if (auto* key = event.getIf<sf::Event::KeyPressed>())
        {

            switch (key->code)
            {
                case sf::Keyboard::Key::Escape:
                    close_requested = true;
                    break;

                case sf::Keyboard::Key::F1:
                    show_debug_info = !show_debug_info;
                    break;

                default:
                    break;
            }
        }
    }
} // namespace