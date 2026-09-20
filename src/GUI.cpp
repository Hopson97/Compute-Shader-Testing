#include "GUI.h"

#include <imgui.h>
#include <imgui_sfml/imgui-SFML.h>
#include <imgui_sfml/imgui_impl_opengl3.h>


namespace GUI
{
    bool init(sf::Window* window)
    {
        return ImGui::SFML::Init(*window, sf::Vector2f{window->getSize()}, false) &&
               ImGui_ImplOpenGL3_Init();
    }

    void begin_frame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
    }

    void shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::SFML::Shutdown();
    }

    void render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void event(const sf::Window& window, sf::Event& e)
    {
        ImGui::SFML::ProcessEvent(window, e);
    }

    void debug_window(const glm::vec3& camera_position, const glm::vec3& camera_rotation)
    {
        auto r = camera_rotation;
        auto p = camera_position;

        // clang-format off
        if (ImGui::Begin("Debug Window"))
        {
            ImGui::Text("Position: (%f, %f, %f)", p.x, p.y, p.z);
            ImGui::Text("Rotation: (%f, %f, %f)", r.x, r.y, r.z);
        }
        // clang-format on

        ImGui::End();
    }

} // namespace GUI