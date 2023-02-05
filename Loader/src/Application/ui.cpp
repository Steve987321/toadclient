#include "toad.h"
#include "Application.h"

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
                // for the flickering of the possible minecraft windows 
                static std::vector<utils::window> shownWindowList = {};
                static float timer = 0;
                static bool count = false;
                if (count)
                {
                    timer += io->DeltaTime;
                    if (timer > 1)
                    {
                        if (shownWindowList.size() != utils::winListVec.size())
                            shownWindowList = utils::winListVec;

                        count = false;
                    }
                }
                if (shownWindowList.size() != utils::winListVec.size() && !count)
                {
                    timer = 0;
                    count = true;
                }

	            if (shownWindowList.empty())
	            {
                    utils::center_text_Multi(ImVec4(0.2f, 0.2f, 0.2f, 1), "make sure minecraft is opened. \n Supported clients: (Lunar Client, Forge or Vanilla)");
	            }
	            else
	            {
                    for (const auto& window : shownWindowList)
                    {
                        auto btn_name = std::string(window.title + " [" + std::to_string(window.pid) + ']');
                        if (btn_name.empty()) btn_name = "##";
                        if (ImGui::Selectable(btn_name.c_str(), true))
                        {
                            if (ImGui::IsMouseDoubleClicked(0))
                            {
	                            
                            }
                            // well do our injection
                        }
                    }
                }
                static bool uok = false;
                if (ImGui::Button("ok"))
                {
                    uok = true;
                }
                if (uok)
                {
                    utils::show_mBox("ok boomer ", "testing the boomer mic\n ok boom boom boom", uok);
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
