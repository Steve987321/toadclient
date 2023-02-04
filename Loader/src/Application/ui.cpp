#include "toad.h"
#include "Application.h"

#include "imgui/imgui.h"

namespace toad
{
    void ui_main(ImGuiIO* io)
    {
	    
    }

    void ui_init(ImGuiIO* io)
    {
	    if (static bool once = false; !once)
        {
#ifdef IMGUI_HAS_VIEWPORT
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->GetWorkPos());
            ImGui::SetNextWindowSize(viewport->GetWorkSize());
            ImGui::SetNextWindowViewport(viewport->ID);
#else
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(io->DisplaySize);
#endif
            once = true;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        ImGui::Begin("select minecraft", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
        {
            utils::center_textX(ImVec4(0.3f, 0.3f, 0.3f, 1), "please select the minecraft window");
            ImGui::BeginChild("mc windows", ImVec2(0, 0), true);
            {
	            if (utils::winList.empty())
	            {
                    utils::center_text(ImVec4(0.2f, 0.2f, 0.2f, 1), "make sure minecraft is opened (Lunar Client, Forge or Vanilla)");
	            }
	            else
	            {
                    for (const auto& window : utils::winList)
                    {
                        auto btn_name = std::string(window.title + " [" + std::to_string(window.pid) + ']');
                        if (btn_name.empty()) btn_name = "##";
                        if (ImGui::Selectable(btn_name.c_str(), true))
                        {
                            // well do our injection
                        }
                    }
                }
            }            
            ImGui::EndChild(); // mc windows
        }
        ImGui::End(); // main window

        ImGui::PopStyleVar();
    }

    void c_Application::render_UI(ImGuiIO* io)
    {
        is_verified ? ui_main(io) : ui_init(io);
    }
}
