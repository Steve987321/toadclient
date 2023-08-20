#pragma once

#ifdef TOAD_LOADER
#include "VisualizeClicker/VisualizeClicker.h"
#include "utils/imfilebrowser.h"
#else
#include "../../Loader/src/Application/VisualizeClicker/VisualizeClicker.h"
#include "../../Loader/src/Application/VisualizeClicker/VisualizeClicker.cpp"
#include "../../Loader/src/utils/imfilebrowser.h"
#endif

namespace toad::ui
{

    // loader ui's
    extern void ui_main(const ImGuiIO* io);
    extern void ui_init(const ImGuiIO* io);

    inline VisualizeClicker vClick;

    // loader extra setting window functions

    extern void clicker_rand_editor(bool* enabled);
    extern void clicker_rand_visualizer(bool* enabled);
    extern void esp_visualizer(bool* enabled);

    // UI for toad when injected
    // function is defined here because it will also get called inside toadll for the internal ui
    inline void UI(const ImGuiIO* io)
    {
        // ui settings
        static bool tooltips = false;

        // clicker extra option rand edit
        static bool clicker_rand_edit = false;
        static bool clicker_rand_visualize = false;

        // esp visuals visualization
        static bool esp_visuals_menu = false;

#ifdef TOAD_LOADER
        constexpr auto window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
#else
        constexpr auto window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
#endif
        ImGui::Begin("main", nullptr, window_flags);
        {
            // Tab Bar
            static int tab = 0;
            ImGui::BeginChild("main Tabs", { 85, 0 });
            {
                //ImGui::ImageButton(); 
                if (ImGui::Button("Combat", { 85, 30 }))
                {
                    tab = 0;
                }
                ImGui::Spacing();
                if (ImGui::Button("Misc", { 85, 30 }))
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
                    static bool is_LClicker = false;
                    static bool is_RClicker = false;
                    static bool is_AA = false;
                    static bool is_Velocity = false;
                    if (checkbox_button("Left Clicker", ICON_FA_MOUSE, &left_clicker::enabled)) is_LClicker = true;
                    ImGui::SameLine(0, 80);
                    checkbox_button("No Click Delay", ICON_FA_CLOCK, &no_click_delay::enabled);
                    if (checkbox_button("Right Clicker", ICON_FA_MOUSE, &right_clicker::enabled)) is_RClicker = true;
                    if (checkbox_button("Aim Assist", ICON_FA_CROSSHAIRS, &aa::enabled)) is_AA = true;
                    if (checkbox_button("Velocity", ICON_FA_WIND, &velocity::enabled)) is_Velocity = true;
                    if (is_LClicker)
                        setting_menu("LeftClicker", is_LClicker, []
                        {
                            if (ImGui::SliderInt("min cps", &left_clicker::min_cps, 5, 20, "%dcps", ImGuiSliderFlags_NoInput))
                            {
                                // update rand delays
                                left_clicker::rand.UpdateDelays(left_clicker::min_cps, left_clicker::max_cps);
                                vClick.SetRand(left_clicker::rand);
                            }

                        	if (ImGui::SliderInt("max cps", &left_clicker::max_cps, 5, 20, "%dcps", ImGuiSliderFlags_NoInput))
                                {
                                    // update rand delays
									left_clicker::rand.UpdateDelays(left_clicker::min_cps, left_clicker::max_cps);
                                    vClick.SetRand(left_clicker::rand);
                                }

		                    ImGui::Checkbox("weapons only", &left_clicker::weapons_only);
		                    ImGui::Checkbox("break blocks", &left_clicker::break_blocks);
		                    ImGui::Checkbox("block hit", &left_clicker::block_hit);
		                    ImGui::SliderInt("block hit delay", &left_clicker::block_hit_ms, 0, 50);
		                    ImGui::Checkbox("smart cps", &left_clicker::targeting_affects_cps);
		                    ImGui::Checkbox("trade assist", &left_clicker::trade_assist);
                        }, true, 
                        []{
                            if (ImGui::Checkbox("Edit Boosts & Drops", &clicker_rand_edit))
                            {
                                auto rand = vClick.GetRand();
                                rand.UpdateDelays(left_clicker::min_cps, left_clicker::max_cps);
                                vClick.SetRand(rand);
                            }
                            else if (ImGui::Checkbox("Visualize Randomization", &clicker_rand_visualize))
                            {
                                auto rand = vClick.GetRand();
                                rand.UpdateDelays(left_clicker::min_cps, left_clicker::max_cps);
                                vClick.SetRand(rand);
                            }
                        });

                    else if (is_RClicker)
                        setting_menu("RightClicker", is_RClicker, []
                        {
                            ImGui::SliderInt("cps", &right_clicker::cps, 1, 20, "%dcps");
		                    ImGui::Checkbox("blocks only", &right_clicker::blocks_only);
		                    ImGui::SliderInt("start delay", &right_clicker::start_delayms, 0, 200, "%dms");
		                });

                    else if (is_AA)
                        setting_menu("Aim Assist", is_AA, []
                        {
                            ImGui::SliderFloat("Speed", &aa::speed, 0, 100);
		                    ImGui::SliderInt("Fov Check", &aa::fov, 0, 360);
		                    ImGui::SliderFloat("Distance", &aa::distance, 0, 10);
		                    ImGui::Checkbox("Horizontal Only", &aa::horizontal_only);
		                    ImGui::Checkbox("Invisibles", &aa::invisibles);
		                    ImGui::Checkbox("Always Aim", &aa::always_aim);
		                    ImGui::Checkbox("Target Lock", &aa::lock_aim);
		                    ImGui::Checkbox("Aim in target", &aa::aim_at_closest_point);
                            ImGui::Checkbox("Break Blocks", &aa::break_blocks);
			                if (ImGui::BeginCombo("target by", AATargetToCStrMap[aa::target_mode]))
			                {
			                    for (const auto& [aaMode, name] : AATargetToCStrMap)
			                    {
			                        if (ImGui::Selectable(name, aaMode == aa::target_mode))
			                            aa::target_mode = aaMode;
			                    }
			                    ImGui::EndCombo();
			                }
                        });

                    else if (is_Velocity)
                        setting_menu("Velocity", is_Velocity, []
                        {
                            ImGui::Checkbox("Use Jump Reset", &velocity::jump_reset);

                            if (velocity::jump_reset)
                            {
                                ImGui::SliderInt("Press Chance", &velocity::jump_press_chance, 0, 100, "%d%%");
                            }

		                    ImGui::Checkbox("Only when moving", &velocity::only_when_moving);
		                    ImGui::Checkbox("Only when clicking", &velocity::only_when_clicking);
		                    ImGui::Checkbox("Kite", &velocity::kite);
		                    if (!velocity::jump_reset)
		                    {
		                        ImGui::SliderFloat("Horizontal", &velocity::horizontal, 0, 100.f, "%.1f%%");
		                        ImGui::SliderFloat("Vertical", &velocity::vertical, 0.f, 100.f, "%.1f%%");
		                        ImGui::SliderInt("Chance", &velocity::chance, 0, 100, "%d%%");
		                        ImGui::SliderFloat("Delay", &velocity::delay, 0, 100, "%.0f%");
		                    }
                        });
                    
                    // ImGui::EndChild();
                }
                else if (tab == 1)
                {
                    static bool is_Bridge = false;
                    static bool is_Esp = false;
                    static bool is_BlockEsp = false;
                    static bool is_Blink = false;
                    if (checkbox_button("Auto Bridge", ICON_FA_CUBE, &bridge_assist::enabled)) is_Bridge = true;
                    if (checkbox_button("ESP", ICON_FA_EYE, &esp::enabled)) is_Esp = true;
                    if (checkbox_button("Block ESP", ICON_FA_CUBES, &block_esp::enabled)) is_BlockEsp = true;
                    if (checkbox_button("Blink", ICON_FA_GHOST, &blink::enabled)) is_Blink = true;

                    if (is_Bridge)
                    {
                        setting_menu("Bridge Assist", is_Bridge, []
                        {
                            ImGui::SliderFloat("pitch check", &bridge_assist::pitch_check, 1, 90);

							// zero and 1 means sneak at any edge
                            ImGui::SliderInt("block height check", &bridge_assist::block_check, 0, 10);
                        });
                    }

                    else if (is_Esp)
                    {
                        constexpr auto color_edit_flags = ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaBar;

                        setting_menu("ESP", is_Esp, []
                        {
                            ImGui::ColorEdit4("Outline Color", esp::line_col, color_edit_flags);
                            ImGui::ColorEdit4("Fill Color", esp::fill_col, color_edit_flags);

                            if (ImGui::BeginCombo("ESP Type", espModeToCStrMap[esp::esp_mode], ImGuiComboFlags_NoArrowButton))
                            {
	                            for (const auto& [espMode, name] : espModeToCStrMap)
	                            {
		                            if (ImGui::Selectable(name, espMode == esp::esp_mode))
			                            esp::esp_mode = espMode;
	                            }
	                            ImGui::EndCombo();
                            }

                            ImGui::Checkbox("open esp settings", &esp_visuals_menu);
                        });
                    }

                    else if (is_BlockEsp)
                    {
                        setting_menu("Block ESP", is_BlockEsp, []
                        {

	                        static std::set<std::string> ignoreSuggestions = {};

	                        static int blockIdInput = 54;
	                        static char buf[20] = "";
	                        ImGui::InputText("search", buf, 20);
	                        ImGui::InputInt("block ID", &blockIdInput, 0, 0, ImGuiInputTextFlags_NoMarkEdited);
	                        if (ImGui::Button("Add"))
	                        {
	                            if (!block_esp::block_list.contains(blockIdInput))
	                            {
	                                block_esp::block_list.insert({ blockIdInput, ImVec4{ 1, 1, 1, 0.3f } });
	                                ignoreSuggestions.insert(ignoreSuggestions.end(), nameOfBlockId[blockIdInput]);
	                                ZeroMemory(buf, 20);
	                            }
	                        }

	                        auto listPos = ImGui::GetCursorPos();
	                        static std::queue<int> removeQueue = {};

	                        ImGui::SetCursorPos(listPos);
	                        ImGui::BeginChild("esp block list", {}, true);

	                        for (auto& [id, col] : block_esp::block_list)
	                        {
	                            ImGui::PushID(id);

	                            // Info
	                            ImGui::Text("%s | %d", nameOfBlockId[id].c_str(), id);

	                            // block color settings
	                            auto& blockEspCol = col;
	                            float coltmp[4] = { blockEspCol.x, blockEspCol.y, blockEspCol.z, blockEspCol.w };

	                            ImGui::SameLine();
	                            ImGui::ColorEdit4("##col", coltmp, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaBar);

	                            blockEspCol = { coltmp[0],coltmp[1],coltmp[2],coltmp[3] };

	                            ImGui::SameLine();
	                            if (ImGui::Button("Remove"))
	                            {
	                                removeQueue.push(id);
	                            }

	                            ImGui::PopID();
	                        }

	                        while (!removeQueue.empty())
	                        {
	                            const auto id = removeQueue.front();
	                            block_esp::block_list.erase(id);
	                            ignoreSuggestions.erase(nameOfBlockId[id]);
	                            removeQueue.pop();
	                        }

	                        ImGui::EndChild();

	                        ImGui::SetCursorPos(listPos);
	                        auto suggestions = get_filtered_suggestions(buf, nameOfBlockId, ignoreSuggestions);
	                        if (!suggestions.empty())
	                        {
	                            ImGui::BeginChild("suggestionlist", { 150, static_cast<float>(suggestions.size() * 20) + 10 }, true);
	                            for (auto& [id, suggested] : suggestions)
	                            {
	                                if (ImGui::Selectable(suggested.c_str(), false))
	                                {
	                                    blockIdInput = id;
	                                    strncpy_s(buf, suggested.c_str(), 20);
	                                }

	                            }
	                            ImGui::EndChild();
	                        }
                        });
                    }

                    else if (is_Blink)
                    {
                        setting_menu("Blink", is_Blink, []
                        {
                            ImGui::InputInt("keycode", &blink::key);
	                        ImGui::InputFloat("max limit in seconds", &blink::limit_seconds, 0, 0, "%.1f");
	                        ImGui::Checkbox("render trail", &blink::show_trail);
	                        ImGui::Checkbox("pause incoming packets", &blink::stop_rec_packets);
                        });
                    }
                }
            } ImGui::EndChild();

            // side extra settings bar

            auto windowright = ImVec2{ImGui::GetContentRegionMax().x + 10,ImGui::GetContentRegionMax().y };

            static bool is_settings_open = false;
            static float setting_bar_t = 1;
            static float setting_bar_posX = windowright.x - 40;
            static float setting_bar_posXsmooth = windowright.x - 40;
            static float setting_bar_alpha = 0;

            if (is_settings_open)
            {
                setting_bar_posXsmooth = std::lerp(windowright.x - 40, setting_bar_posX, setting_bar_t);
                setting_bar_alpha = std::lerp(0, 1, setting_bar_t);
            }
            else
            {
                setting_bar_posXsmooth = std::lerp(windowright.x - 150, setting_bar_posX, setting_bar_t);
                setting_bar_alpha = std::lerp(1, 0, setting_bar_t);
            }
            ImGui::SetCursorPosX(setting_bar_posXsmooth);
            ImGui::SetCursorPosY(5);
            const static auto border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
            //const static auto childbg_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
            //ImGui::PushStyleColor(ImGuiCol_ChildBg, { childbg_col.x + 0.1f, childbg_col.y + 0.1f, childbg_col.z + 0.1f, childbg_col.w });
            ImGui::PushStyleColor(ImGuiCol_Border, { border_col.x, border_col.y, border_col.z, setting_bar_alpha });
            ImGui::BeginChild("settings bar", { 150, windowright.y - 20 }, true);
            {
                ImGui::PopStyleColor(1);
                ImGui::SetCursorPosY(20);
                ImGui::PushID("Settings");
                ImGui::Text(ICON_FA_BARS);
                ImGui::SetCursorPos({ ImGui::GetCursorPosX() - 10, ImGui::GetCursorPosY() - 28 });
                ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1,1,1,0.2f });
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1,1,1,0.1f });
                if (ImGui::Button("##", { 35, 24 }))
                {
                    if (!is_settings_open)
                    {
                        is_settings_open = true;
                        setting_bar_posX = windowright.x - 150;
                        setting_bar_t = 0;
                    }
                    else
                    {
                        is_settings_open = false;
                        setting_bar_posX = windowright.x - 40;
                        setting_bar_t = 0;
                    }
                }
                ImGui::PopStyleColor(3);

                ImGui::PopID();
                ImGui::SameLine();
                ImGui::BeginChild("Settings");
                {
                    ImGui::Checkbox("tooltips", &tooltips);

#ifdef TOAD_LOADER
                    if (ImGui::Button("internal ui"))
#else
                    if (ImGui::Button("external ui"))
#endif
                    {
#ifdef TOAD_LOADER
                        g_is_ui_internal = true;
                        ShowWindow(AppInstance->GetWindow()->GetHandle(), SW_HIDE);
#else
                        LOGDEBUG("Closing Internal UI and switching to Loader's UI");
                        toadll::CInternalUI::ShouldClose = true;
#endif
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();

            if (is_settings_open && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && setting_bar_t > 0.1f)
                if (ImGui::IsMouseHoveringRect({ 0,0 }, { ImGui::GetWindowPos().x + setting_bar_posX, ImGui::GetWindowPos().y + windowright.y }))
                {
                    is_settings_open = false;
                    setting_bar_posX = windowright.x - 40;
                    setting_bar_t = 0;
                }

            if (setting_bar_t < 0.99f)
            {
                setting_bar_t += io->DeltaTime * 5.f;
                if (setting_bar_t >= 1.0f)
                {
                    setting_bar_t = 1;
                }
            }
        }

        if (clicker_rand_edit)
        {
            clicker_rand_editor(&clicker_rand_edit);
        }

        if (clicker_rand_visualize)
        {
            clicker_rand_visualizer(&clicker_rand_visualize);
        }

        if (esp_visuals_menu)
        {
            esp_visualizer(&esp_visuals_menu);
        }

        ImGui::End();
    }
    
}
