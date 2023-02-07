#include "toad.h"
#include "Application.h"
#include "inject/injector.h"

namespace toad
{
    // ui when injected
    void ui_main(ImGuiIO* io)
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

        static int tab = 0;

        ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
        {
            // Tab Bar
            //ImGui::ImageButton();
            if (ImGui::Button("Combat"))
            {
                tab = 0;
            }
            else if (ImGui::Button("Visuals"))
            {
                tab = 1;
            }
            else if (ImGui::Button("Misc"))
            {
                tab = 2;
            }

            // 
            
            if (tab == 0)
            {
                utils::groupBox("yes");

                ImGui::Checkbox("Enabled", &aa::enabled);
                ImGui::SliderFloat("Speed", &aa::speed, 0, 100);
                ImGui::SliderFloat("Distance", &aa::distance, 0, 10);

                ImGui::EndChild();
            }
            else if (tab == 1)
            {
	            
            }
            else if (tab == 2)
            {
	            
            }

        }
    	ImGui::End();
    }

    // ui when not injected 
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
            ImGui::BeginChild("mc windows", ImVec2(0, 0), true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
            {
                // for the flickering of the possible minecraft windows 
                static std::vector<utils::window> shownWindowList = {};
                static float timer = 0;
                static bool count = false;
                static bool failed_shared_mem = false;
                static bool failed_inject = false;
                static bool loading = false;

                static std::thread init_thread;

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
                        if (ImGui::Button(btn_name.c_str()))
                        {
                            loading = true;
                            init_thread = std::thread([=]
                                {
                                    if (!init())
                                    failed_shared_mem = true;
									if (!failed_shared_mem)
										 if (!inject(window.pid))
                                             failed_inject = true;
                                    if (!failed_shared_mem && !failed_inject)
                                        is_verified = true;
                                });                                        
                        }
                    }
                }
                if (loading)
                {
                    std::string state = "state: " + inject_status;
                    utils::show_mBox("Injecting", state.c_str(), loading, false);
                    ImGui::SetCursorPos({ utils::get_middle_point().x - 10, utils::get_middle_point().y - 10 });
                    utils::load_spinner("##injectingSpinner", 10, 3, IM_COL32(100, 0, 0, 255));
                }
                else if (failed_shared_mem)
                {
                    if (init_thread.joinable()) init_thread.join();
                    loading = false;
                    utils::show_mBox("failed", "failed to initialize", failed_shared_mem, true, utils::mboxType::ERR);
                }
                else if (failed_inject)
                {
                    if (init_thread.joinable()) init_thread.join();
                    loading = false;
                    utils::show_mBox("failed", "failed to inject", failed_shared_mem, true, utils::mboxType::ERR);
                }
                ImGui::Checkbox("Debug", &dll_debug_mode);
            }
            ImGui::EndChild(); // mc windows
        }
        ImGui::End(); // main window

        ImGui::PopStyleVar();
    }

    void c_Application::render_UI(ImGuiIO* io)
    {
        is_verified ? ui_main(io) : ui_init(io);
        //ui_main(io);
    }
}
