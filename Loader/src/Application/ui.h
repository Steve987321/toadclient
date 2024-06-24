#pragma once

#ifdef TOAD_LOADER
#include "VisualizeClicker/visualize_clicker.h"
#include "utils/imfilebrowser.h"
#include "config.h"
#else
#include "../../Loader/src/Application/VisualizeClicker/visualize_clicker.h"
#include "../../Loader/src/Application/VisualizeClicker/visualize_clicker.cpp"
#include "../../Loader/src/Application/config.h"
#include "../../Loader/src/utils/imfilebrowser.h"
#endif

namespace toad::ui
{

    // loader ui's
    extern void ui_main(const ImGuiIO* io);
    extern void ui_init(const ImGuiIO* io);

    inline VisualizeClicker visual_clicker;

    // loader extra setting window functions
    extern void clicker_rand_editor(bool* enabled);
    extern void clicker_rand_visualizer(bool* enabled);
    extern void esp_visualizer(bool* enabled);

    // UI for toad when injected
    // function is defined here because it will also get called inside toadll for the internal ui
    inline void UI(const ImGuiIO* io)
    {
        // ui settings
        // #TODO: implement
        static bool tooltips = false;

        // clicker extra option rand edit
        static bool clicker_rand_edit = false;
        static bool clicker_rand_visualize = false;

        // esp visuals visualization
        static bool esp_visuals_menu = false;

        // config
        static std::vector<config::ConfigFile> available_configs = {};

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
                constexpr ImVec2 btn_size = { 85, 30 };

                if (ImGui::Button("Combat", btn_size))
                {
                    tab = 0;
                }

                ImGui::Spacing();

                if (ImGui::Button("Misc", btn_size))
                {
                    tab = 1;
                }

                ImGui::Spacing();

                if (ImGui::Button("Config", btn_size))
                {
                    tab = 2;
                    if (available_configs.empty())
                    {
                        available_configs = config::GetAllConfigsInDirectory(loader_path);
                    }
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
                    static bool menu_lc = false;
                    static bool menu_rc = false;
                    static bool menu_aa = false;
                    static bool menu_vel = false;
                    if (checkbox_button("Left Clicker", ICON_FA_MOUSE, &left_clicker::enabled)) menu_lc = true;
                    if (g_curr_client == MC_CLIENT::Lunar_189)
                    {
                        ImGui::SameLine(0, 80);
                        checkbox_button("No Hit Delay", ICON_FA_CLOCK, &no_click_delay::enabled);
                    }
                    if (checkbox_button("Right Clicker", ICON_FA_MOUSE, &right_clicker::enabled)) menu_rc = true;
                    if (checkbox_button("Aim Assist", ICON_FA_CROSSHAIRS, &aa::enabled)) menu_aa = true;
                    if (checkbox_button("Velocity", ICON_FA_WIND, &velocity::enabled)) menu_vel = true;
                    if (menu_lc)
                        setting_menu("LeftClicker", menu_lc, []
                        {
                            if (ImGui::SliderInt("min cps", &left_clicker::min_cps, 5, 20, "%dcps", ImGuiSliderFlags_NoInput))
                            {
                                // update rand delays
                                left_clicker::rand.UpdateDelays(left_clicker::min_cps, left_clicker::max_cps);
                                visual_clicker.SetRand(left_clicker::rand);
                            }

                        	if (ImGui::SliderInt("max cps", &left_clicker::max_cps, 5, 20, "%dcps", ImGuiSliderFlags_NoInput))
                                {
                                    // update rand delays
									left_clicker::rand.UpdateDelays(left_clicker::min_cps, left_clicker::max_cps);
                                    visual_clicker.SetRand(left_clicker::rand);
                                }

		                    ImGui::Checkbox("weapons only", &left_clicker::weapons_only);
		                    ImGui::Checkbox("break blocks", &left_clicker::break_blocks);
		                    ImGui::Checkbox("block hit", &left_clicker::block_hit);
		                    ImGui::Checkbox("smart cps", &left_clicker::targeting_affects_cps);
		                    ImGui::Checkbox("trade assist", &left_clicker::trade_assist);
                            if (ImGui::BeginCombo("click check", clickCheckToCStrMap[left_clicker::click_check], ImGuiComboFlags_NoArrowButton))
                            {
                                for (const auto& [clickCheck, name] : clickCheckToCStrMap)
                                {
                                    if (ImGui::Selectable(name, clickCheck == left_clicker::click_check))
                                        left_clicker::click_check = clickCheck;
                                }
                                ImGui::EndCombo();
                            }
                        }, true, 
                        []{
                            if (ImGui::Checkbox("Edit Boosts & Drops", &clicker_rand_edit))
                            {
                                auto rand = visual_clicker.GetRand();
                                rand.UpdateDelays(left_clicker::min_cps, left_clicker::max_cps);
                                visual_clicker.SetRand(rand);
                            }
                            else if (ImGui::Checkbox("Visualize Randomization", &clicker_rand_visualize))
                            {
                                auto rand = visual_clicker.GetRand();
                                rand.UpdateDelays(left_clicker::min_cps, left_clicker::max_cps);
                                visual_clicker.SetRand(rand);
                            }

							ImGui::Spacing();

	                        ImGui::Text("break blocks");
	                        ImGui::Separator();
	                        ImGui::SliderInt("start delay", &left_clicker::start_break_blocks_reaction, 30, 500, "%dms");
	                        ImGui::SliderInt("stop delay", &left_clicker::stop_break_blocks_reaction, 30, 500, "%dms");

                            ImGui::Spacing();

	                        ImGui::Text("block hit");
                            ImGui::Separator();
                            ImGui::Checkbox("pause left click", &left_clicker::block_hit_stop_lclick);
	                        ImGui::SliderInt("block hit delay", &left_clicker::block_hit_ms, 10, 200);
                        });

                    else if (menu_rc)
                        setting_menu("RightClicker", menu_rc, []
                        {
                            ImGui::SliderInt("cps", &right_clicker::cps, 1, 20, "%dcps");
		                    ImGui::Checkbox("blocks only", &right_clicker::blocks_only);
		                    ImGui::SliderInt("start delay", &right_clicker::start_delayms, 0, 200, "%dms");
                            if (ImGui::BeginCombo("click check", clickCheckToCStrMap[right_clicker::click_check], ImGuiComboFlags_NoArrowButton))
                            {
                                for (const auto& [clickCheck, name] : clickCheckToCStrMap)
                                {
                                    if (ImGui::Selectable(name, clickCheck == right_clicker::click_check))
                                        right_clicker::click_check = clickCheck;
                                }
                                ImGui::EndCombo();
                            }
		                });

                    else if (menu_aa)
                        setting_menu("Aim Assist", menu_aa, []
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

                    else if (menu_vel)
                        setting_menu("Velocity", menu_vel, []
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
		                        ImGui::SliderInt("Delay", &velocity::delay, 0, 10);
		                    }
                        });
                }
                else if (tab == 1)
                {
                    static bool is_Bridge = false;
                    static bool is_Esp = false;
                    static bool is_BlockEsp = false;
                    static bool is_Blink = false;
                    static bool is_ArrayList = false;
                    static bool is_ChestStealer = false;
                    if (checkbox_button("Auto Bridge", ICON_FA_CUBE, &bridge_assist::enabled)) is_Bridge = true;
					ImGui::SameLine(0, 80);
					if (checkbox_button("Array List", ICON_FA_BARS, &ui::show_array_list)) is_ArrayList = true;
                    if (checkbox_button("ESP", ICON_FA_EYE, &esp::enabled)) is_Esp = true;
                    ImGui::SameLine(0, 80);
					if (checkbox_button("Chest Stealer", ICON_FA_JOINT, &chest_stealer::enabled)) is_ChestStealer = true;
                    if (checkbox_button("Block ESP", ICON_FA_CUBES, &block_esp::enabled)) is_BlockEsp = true;
                    if (checkbox_button("Blink", ICON_FA_GHOST, &blink::enabled)) is_Blink = true;

                    if (is_Bridge)
                    {
                        setting_menu("Bridge Assist", is_Bridge, []
                        {
                            ImGui::SliderFloat("pitch check", &bridge_assist::pitch_check, 1, 90);

							// zero and 1 means sneak at any edge
                            ImGui::SliderInt("block height check", &bridge_assist::block_check, 0, 10);

                            ImGui::Checkbox("Only initiate when sneaking", &bridge_assist::only_initiate_when_sneaking);
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

                    else if (is_ArrayList)
                    {
                        setting_menu("Array List", is_ArrayList, []
                            {
                                // #TODO: add array list options
                                center_text_multi({1,1,1,1}, "WIP. \n customisation options will be \n added here later");
                            });
                    }

                    else if (is_ChestStealer)
                    {
						setting_menu("Chest Stealer", is_ChestStealer, []
							{
                                //ImGui::Checkbox("show grid", &chest_stealer::show_slot_positions);
                                ImGui::SliderInt("delay ms", &chest_stealer::average_slowness_ms, 10, 200);

								static char buf[64];
								static char rename_buf[64];
								static int selected_item_index = -1;
								static bool rename_item = false;
                                static ImGuiID cs_items_popup = ImHashStr("POPUP_CS_ITEMS");

                                if (ImGui::InputText("item", buf, 64, ImGuiInputTextFlags_EnterReturnsTrue))
                                {
                                    chest_stealer::items_to_grab.emplace_back(buf);
                                }
                                ImGui::SameLine();
                                ImGui::BeginDisabled(strlen(buf) == 0);
                                if (ImGui::Button("Add"))
                                {
									chest_stealer::items_to_grab.emplace_back(buf);
                                }
                                ImGui::EndDisabled();

                                ImGui::BeginChild("Items", {0, 0}, ImGuiChildFlags_Border);
                                {
                                    for (size_t i = 0; i < chest_stealer::items_to_grab.size(); i++)
                                    {
                                        std::string& item = chest_stealer::items_to_grab[i];

                                        if (i == selected_item_index && rename_item)
                                        {
											if (ImGui::InputText("item", rename_buf, 64, ImGuiInputTextFlags_EnterReturnsTrue))
											{
                                                item = rename_buf;
                                                memset(rename_buf, '\0', 64);
                                                rename_item = false;
											}
                                        }
                                        else
                                        {
											ImGui::Selectable(item.c_str());
                                        }
                                        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                                        {
											ImGui::PushOverrideID(cs_items_popup);
                                            ImGui::OpenPopup("POPUP_CS_ITEMS");
											ImGui::PopID();
                                            selected_item_index = (int)i;
                                            strcpy_s(rename_buf, item.c_str());
                                        }
                                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                                        {
											selected_item_index = (int)i;
											strcpy_s(rename_buf, item.c_str());
                                            rename_item = true;
                                        }
                                    }
                                }
                                ImGui::EndChild();

                                ImGui::PushOverrideID(cs_items_popup);
								if (ImGui::BeginPopup("POPUP_CS_ITEMS"))
								{
									if (ImGui::MenuItem("Delete"))
									{
										chest_stealer::items_to_grab.erase(chest_stealer::items_to_grab.begin() + selected_item_index);
									}
									if (ImGui::MenuItem("Rename"))
									{
										rename_item = true;
									}
									ImGui::EndPopup();
								}
                                ImGui::PopID();

                                center_text_multi({ 1, 1, 0, 1 }, "This feature is WIP. \n only works on small chests. \n Or not at all.");
							});
                    }
                }
                else if (tab == 2)
                {
                    static char searching_dir[MAX_PATH];
                    static char config_name_buf[50] = {};

                    static bool once = false;
                    if (!once)
                    {
                        memcpy(searching_dir, loader_path.c_str(), loader_path.length());
                        memcpy(config_name_buf, loaded_config.c_str(), loaded_config.length());
                        once = true;
                    }

                    ImGui::BeginChild("config list", {ImGui::GetWindowSize().x / 2 - 5, 0 }, true);
                    {
                        ImGui::BeginChild("##available configs", {0, 150}, true);
                        {
                            const auto size_x = ImGui::CalcTextSize(searching_dir).x;
                            ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x / 2 - size_x / 2);
                            if (ImGui::Button(searching_dir))
                            {
                                config::GetAllConfigsInDirectory(searching_dir);
                            }

                            if (size_x > ImGui::GetContentRegionMax().x)
                            {
                                if (ImGui::IsItemHovered())
                                {
                                    ImGui::SetTooltip(searching_dir);
                                }
                            }

                            ImGui::Separator();

                            if (available_configs.empty())
                            {
                                center_text_multi({ 1, 0, 0, 0.5f }, 
								"No configs found,\n"
								"file ext must be .toad\n"
                                );
                            }
                            else
                            {
                                static int selected = -1;

	                            for (int i = 0; i < available_configs.size(); i++)
	                            {
                                    const auto& [file_name, time_point] = available_configs[i];

                                    auto label = file_name + " | " + time_to_str(time_point, "%d-%m-%Y %H:%M");

                                    if (ImGui::Selectable(label.c_str(), selected == i))
                                    {
                                        memcpy(config_name_buf, file_name.c_str(), file_name.length());
                                        selected = i;
                                    }
                                    if (ImGui::IsItemHovered())
                                    {
                                        ImGui::SetTooltip(label.c_str());
                                    }
	                            }
                            }
                            ImGui::EndChild();
                        }
                        ImGui::Separator();

                        if (ImGui::Button("Refresh"))
                        {
                            available_configs = config::GetAllConfigsInDirectory(searching_dir);
                        }

                        ImGui::BeginDisabled(strlen(config_name_buf) == 0);
                        if (ImGui::Button("Load"))
                        {
                            config::LoadConfig(searching_dir, config_name_buf);
                        }

                        if (ImGui::Button("Save"))
                        {
                            config::SaveConfig(searching_dir, config_name_buf);
                        }
                        ImGui::EndDisabled();

                        ImGui::InputText("name", config_name_buf, 50);

                        ImGui::EndChild();
                    }

                    ImGui::SameLine(0, 5);

                    ImGui::BeginChild("##config selection menu", {ImGui::GetContentRegionAvail().x - 30, 0}, true);
                    {
                        if (ImGui::Button("Load from Clipboard"))
                        {
                            config::LoadConfigFromClipBoard();
                        }
                        if (ImGui::Button("Save to Clipboard"))
                        {
                            config::SaveConfigToClipBoard();
                        }
                        ImGui::EndChild();
                    }

                }
            } ImGui::EndChild();

            // side extra settings bar

            auto window_right = ImVec2{ImGui::GetContentRegionMax().x + 10,ImGui::GetContentRegionMax().y };

            static bool is_settings_open = false;
            static float setting_bar_t = 1;
            static float setting_bar_posX = window_right.x - 40;
            static float setting_bar_posXsmooth = window_right.x - 40;
            static float setting_bar_alpha = 0;

            if (is_settings_open)
            {
                setting_bar_posXsmooth = std::lerp(window_right.x - 40, setting_bar_posX, setting_bar_t);
                setting_bar_alpha = std::lerp(0.f, 1.f, setting_bar_t);
            }
            else
            {
                setting_bar_posXsmooth = std::lerp(window_right.x - 150, setting_bar_posX, setting_bar_t);
                setting_bar_alpha = std::lerp(1.f, 0.f, setting_bar_t);
            }
            ImGui::SetCursorPosX(setting_bar_posXsmooth);
            ImGui::SetCursorPosY(5);
            const static auto border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
            const static auto childbg_col = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, { childbg_col.x, childbg_col.y, childbg_col.z, setting_bar_alpha });
            ImGui::PushStyleColor(ImGuiCol_Border, { border_col.x, border_col.y, border_col.z, setting_bar_alpha });
            ImGui::BeginChild("settings bar", { 150, window_right.y - 20 }, true);
            {
                ImGui::PopStyleColor(2);
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
                        setting_bar_posX = window_right.x - 150;
                        setting_bar_t = 0;
                    }
                    else
                    {
                        is_settings_open = false;
                        setting_bar_posX = window_right.x - 40;
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
                        ShowWindow(Application::Get()->GetWindow()->GetHandle(), SW_HIDE);
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
                if (ImGui::IsMouseHoveringRect({ 0,0 }, { ImGui::GetWindowPos().x + setting_bar_posX, ImGui::GetWindowPos().y + window_right.y }))
                {
                    is_settings_open = false;
                    setting_bar_posX = window_right.x - 40;
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
        else
        {
            if (visual_clicker.IsStarted())
                visual_clicker.Stop();
        }

        if (esp_visuals_menu)
        {
            esp_visualizer(&esp_visuals_menu);
        }

        ImGui::End();
    }
    
}
