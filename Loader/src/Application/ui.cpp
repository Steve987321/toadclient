#include "toad.h"
#include "Application.h"
#include "inject/injector.h"

std::thread init_thread;

namespace toad
{
    // ui when injected
    void ui_main(const ImGuiIO* io)
    {
        if (static bool once = false; !once)
        {
            if (init_thread.joinable()) init_thread.join();
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

        ui::UI(io);
    }

    // ui when not injected 
    void ui_init(const ImGuiIO* io)
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
                static bool invalid_client_type = false;
                static bool loading = false;

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
                    utils::center_text_Multi(ImVec4(0.2f, 0.2f, 0.2f, 1), "make sure minecraft is opened. \n Supported clients: Lunar Client (1.8.9 & 1.7.10)");
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

                            // get client type
                            inject_status = "getting client type";

                            g_curr_client = utils::GetClientType(window.title);
                            std::cout << "currclient type: " << (int)g_curr_client << std::endl;
                            if (g_curr_client != MC_CLIENT::NOT_SUPPORTED)
                            {
                                init_thread = std::thread([&]
                                    {
                                        inject_status = "init #1";

                                        if (!init())
                                        failed_shared_mem = true;
											
		                                if (!failed_shared_mem)
		                                    if (!inject(window.pid))
		                                        failed_inject = true;

		                                if (!failed_shared_mem && !failed_inject)
		                                    g_is_verified = true;

										loading = false;
                                    });
                            }
                            else
                            {
                                invalid_client_type = true;
                                loading = false;
                            }
                        }
                    }
                }
                if (loading)
                {
                    std::string state = "state: " + inject_status;
                    utils::show_mBox("Injecting", state.c_str(), loading, false);
                    ImGui::SetCursorPos({ utils::get_middle_point().x - 10, utils::get_middle_point().y - 10 });
                    utils::load_spinner("##injectingSpinner", 10, 3, IM_COL32(100, 100, 100, 255));
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
                    utils::show_mBox("failed", "failed to inject", failed_inject, true, utils::mboxType::ERR);
                }
                else if (invalid_client_type)
                {
                    if (init_thread.joinable()) init_thread.join();
                    loading = false;
                    utils::show_mBox("failed", "client is not supported", invalid_client_type, true, utils::mboxType::ERR);
                }
                ImGui::Checkbox("Debug", &g_dll_debug_mode);
            }
            ImGui::EndChild(); // mc windows
        }
        ImGui::End(); // main window

        ImGui::PopStyleVar();
    }

    void CApplication::render_UI()
    {
#ifdef _DEBUG
        ui_main(io);
#else
        g_is_verified ? ui_main(io) : ui_init(io);
#endif
    }
}
