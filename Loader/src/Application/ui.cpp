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

        ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
        {
	        static int tab = 0;
	        ImGui::BeginChild("main Tabs", {85, 0});
            {
                // Tab Bar
                //ImGui::ImageButton();
                if (ImGui::Button("Combat", { 85, 75 }))
                {
                    tab = 0;
                }
                ImGui::Spacing();
                if (ImGui::Button("Misc", { 85, 75 }))
                {
                    tab = 1;
                }
            } ImGui::EndChild();

            ImGui::SameLine();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
            ImGui::SameLine();

            //

            ImGui::BeginChild("Selected Tab Modules");
            {
                if (tab == 0)
                {
                    static bool is_Clicker = false;
                    static bool is_AA = false;
                    static bool is_Velocity = false;
                    if (utils::checkboxButton("Clicker", ICON_FA_MOUSE, &clicker::enabled)) is_Clicker = true;
                    if (utils::checkboxButton("Aim Assist", ICON_FA_CROSSHAIRS, &aa::enabled)) is_AA = true;
                    if (utils::checkboxButton("Velocity", ICON_FA_WIND, &velocity::enabled)) is_Velocity = true;

                    if (is_Clicker)
                        utils::setting_menu("Auto Clicker", is_Clicker, []
                            {
                                ImGui::SliderInt("cps", &clicker::cps, 0, 20, "%dcps");
			                    ImGui::Checkbox("weapons only", &clicker::weapons_only);
                                ImGui::Checkbox("break blocks", &clicker::break_blocks);
			                    ImGui::Checkbox("block hit", &clicker::block_hit);
			                    ImGui::SliderInt("block hit delay", &clicker::block_hit_ms, 0, 50);
			                    ImGui::Checkbox("smart cps", &clicker::targeting_affects_cps);
			                    ImGui::Checkbox("trade assist", &clicker::trade_assist);
                            });
                    else if (is_AA)
                        utils::setting_menu("Aim Assist", is_AA, []
                            {
                                ImGui::SliderFloat("Speed", &aa::speed, 0, 10);
			                    ImGui::SliderInt("Fov Check", &aa::fov, 0, 360);
			                    ImGui::SliderFloat("Distance", &aa::distance, 0, 10);
			                    ImGui::Checkbox("Horizontal Only", &aa::horizontal_only);
			                    ImGui::Checkbox("Invisibles", &aa::invisibles);
			                    ImGui::Checkbox("Always Aim", &aa::always_aim);
                                ImGui::Checkbox("Target Lock", &aa::lock_aim);
                                ImGui::Checkbox("Multi Point", &aa::aim_at_closest_point);
			                    ImGui::Text("Target by: %s", aa::targetFOV ? "fov" : "distance");
			                    if (ImGui::IsItemClicked())
			                        aa::targetFOV = !aa::targetFOV;
                            });
                    else if (is_Velocity)
                        utils::setting_menu("Velocity", is_Velocity, []
                            {
                                ImGui::SliderFloat("Horizontal", &velocity::horizontal, 0, 100.f, "%.1f%%");
                                ImGui::SliderFloat("Vertical", &velocity::vertical, 0.f, 100.f, "%.1f%%");
                                ImGui::SliderInt("Chance", &velocity::chance, 0, 100, "%d%%");
                                ImGui::SliderFloat("Delay", &velocity::delay, 0, 100, "%.0f%");
                        });

                    // ImGui::EndChild();
                }
                else if (tab == 1)
                {
                    static bool is_Bridge = false;
                    if (utils::checkboxButton("Auto Bridge", ICON_FA_CUBE, &auto_bridge::enabled)) is_Bridge = true;

                    if (is_Bridge)
                    {
                        utils::setting_menu("Auto Bridge", is_Bridge, []
                            {

                            ImGui::SliderFloat("pitch check", &auto_bridge::pitch_check, 1, 70);

                            // animation of how the bridging might look with settings
	                      /*  ImDrawList* draw = ImGui::GetForegroundDrawList();
                            ImGuiContext& g = *GImGui;
                            auto pos = ImVec2{300,200};
	                        draw->AddRect(pos, { pos.x + 20, pos.y + 20 }, IM_COL32(100, 100, 100, 255));
                            draw->AddRect({ pos.x, pos.y + 20 }, { pos.x + 20, pos.y + 60 }, IM_COL32(100, 100, 100, 255));
                            draw->AddRect({ pos.x, pos.y + 60 }, { pos.x + 20, pos.y + 120 }, IM_COL32(100, 100, 100, 255));
                            draw->AddRect({ pos.x, pos.y + 60 }, { pos.x + 20, pos.y + 120 }, IM_COL32(100, 100, 100, 255));*/

                            });
                    }
                }
            } ImGui::EndChild();
        }
    	ImGui::End();
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
                                        g_is_verified = true;

                                    loading = false;
                                });                                        
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
                ImGui::Checkbox("Debug", &g_dll_debug_mode);
            }
            ImGui::EndChild(); // mc windows
        }
        ImGui::End(); // main window

        ImGui::PopStyleVar();
    }

    void c_Application::render_UI()
    {
#ifdef _DEBUG
        ui_main(io);
#else
        g_is_verified ? ui_main(io) : ui_init(io);
#endif
    }
}
