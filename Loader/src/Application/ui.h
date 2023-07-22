#pragma once

namespace toad::ui
{
    void ui_main(const ImGuiIO* io);
    void ui_init(const ImGuiIO* io);

	inline void UI(const ImGuiIO* io)
	{

        // ui settings
        static bool tooltips = false;
#ifdef TOAD_LOADER
        ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
#else
        ImGui::Begin("main", nullptr, ImGuiWindowFlags_None | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
#endif
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
                    if (checkbox_button("Right Clicker", ICON_FA_MOUSE, &right_clicker::enabled)) is_RClicker = true;
                    if (checkbox_button("Aim Assist", ICON_FA_CROSSHAIRS, &aa::enabled)) is_AA = true;
                    if (checkbox_button("Velocity", ICON_FA_WIND, &velocity::enabled)) is_Velocity = true;

                    if (is_LClicker)
                        setting_menu("LeftClicker", is_LClicker, []
                            {
                                ImGui::SliderInt("cps", &left_clicker::cps, 0, 20, "%dcps");
			                    ImGui::Checkbox("weapons only", &left_clicker::weapons_only);
			                    ImGui::Checkbox("break blocks", &left_clicker::break_blocks);
			                    ImGui::Checkbox("block hit", &left_clicker::block_hit);
			                    ImGui::SliderInt("block hit delay", &left_clicker::block_hit_ms, 0, 50);
			                    ImGui::Checkbox("smart cps", &left_clicker::targeting_affects_cps);
			                    ImGui::Checkbox("trade assist", &left_clicker::trade_assist);
                            }, true, 
                            []
                            {
                                ImGui::Text("this is jesus");
                            });

                    else if (is_RClicker)
                        setting_menu("RightClicker", is_RClicker, []
                            {
                                ImGui::SliderInt("cps", &right_clicker::cps, 0, 20, "%dcps");
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
			                    ImGui::Checkbox("Multi Point", &aa::aim_at_closest_point);
			                    ImGui::Text("Target by: %s", aa::targetFOV ? "fov" : "distance");
			                    if (ImGui::IsItemClicked())
			                        aa::targetFOV = !aa::targetFOV;
                            });

                    else if (is_Velocity)
                        setting_menu("Velocity", is_Velocity, []
                            {
                    ImGui::Checkbox("Use Jump Reset", &velocity::jump_reset);
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
                    static bool is_Esp = false;
                    static bool is_Blink = false;
                    if (checkbox_button("Auto Bridge", ICON_FA_CUBE, &auto_bridge::enabled)) is_Bridge = true;
                    if (checkbox_button("ESP", ICON_FA_EYE, &esp::enabled)) is_Esp = true;
                    if (checkbox_button("Blink", ICON_FA_GHOST, &blink::enabled)) is_Blink = true;

                    if (is_Bridge)
                    {
                        setting_menu("Auto Bridge", is_Bridge, []
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

                    else if (is_Esp)
                    {
                        setting_menu("ESP", is_Esp, []
                            {
                        ImGui::ColorEdit4("Outline Color", esp::lineCol, ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaBar);
                        ImGui::ColorEdit4("Fill Color", esp::fillCol, ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaBar);
                        ImGui::Checkbox("Block Esp", &block_esp::enabled);

                        static std::set<std::string> ignoreSuggestions = {};

                        if (block_esp::enabled)
                        {
                            static int blockIdInput = 54;
                            static char buf[20] = "";
                            ImGui::SetNextItemWidth(50);
                            ImGui::InputText("search", buf, 20);
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(50);
                            ImGui::InputInt("block ID", &blockIdInput, 0, 0, ImGuiInputTextFlags_NoMarkEdited);
                            ImGui::SameLine();
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
                                    block_esp::block_list.erase(id);
                                    ignoreSuggestions.erase(nameOfBlockId[id]);
                                }

                                ImGui::PopID();

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
                            }
                        );
                    }
                }
            } ImGui::EndChild();

            // side extra settings bar 
            static bool is_settings_open = false;
            static float setting_bar_t = 1;
        	static float setting_bar_posX = io->DisplaySize.x - 40;
            static float setting_bar_posXsmooth = io->DisplaySize.x - 40;
            static float setting_bar_alpha = 0;

            if (is_settings_open)
            {
                setting_bar_posXsmooth = std::lerp(io->DisplaySize.x - 40, setting_bar_posX, setting_bar_t);
                setting_bar_alpha = std::lerp(0, 1, setting_bar_t);
            }
            else
            {
                setting_bar_posXsmooth = std::lerp(io->DisplaySize.x - 150, setting_bar_posX, setting_bar_t);
                setting_bar_alpha = std::lerp(1, 0, setting_bar_t);
            }
            ImGui::SetCursorPosX(setting_bar_posXsmooth);
            ImGui::SetCursorPosY(5);
            const static auto border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
            //const static auto childbg_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
            //ImGui::PushStyleColor(ImGuiCol_ChildBg, { childbg_col.x + 0.1f, childbg_col.y + 0.1f, childbg_col.z + 0.1f, childbg_col.w });
            ImGui::PushStyleColor(ImGuiCol_Border, {border_col.x, border_col.y, border_col.z, setting_bar_alpha});
            ImGui::BeginChild("settings bar", { 150, io->DisplaySize.y - 20}, true);
            {
                ImGui::PopStyleColor(1);
                ImGui::SetCursorPosY(20);
                ImGui::PushID("Settings");
                ImGui::Text(ICON_FA_BARS);
                ImGui::SetCursorPos({ImGui::GetCursorPosX() - 10, ImGui::GetCursorPosY() - 28});
                ImGui::PushStyleColor(ImGuiCol_Button, {0,0,0,0});
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1,1,1,0.2f});
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1,1,1,0.1f});
                if (ImGui::Button("##", {35, 24}))
                {
                    if (!is_settings_open)
                    {
                        is_settings_open = true;
                        setting_bar_posX = io->DisplaySize.x - 150;
                        setting_bar_t = 0;
                    }
                    else
                    {
                        is_settings_open = false;
                        setting_bar_posX = io->DisplaySize.x - 40;
                        setting_bar_t = 0;
                    }
                }
                ImGui::PopStyleColor(3);

                ImGui::PopID();
                ImGui::SameLine();
                ImGui::BeginChild("Settings");
                {
                    ImGui::Checkbox("tooltips", &tooltips);

                    if (ImGui::Button("internal ui"))
                    {
                        g_is_ui_internal = true;
                        ShowWindow(AppInstance->GetWindow(), SW_HIDE);
                    }
                }
                ImGui::EndChild();
            }
            ImGui::EndChild();

            if (is_settings_open && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && setting_bar_t > 0.1f)
                if (ImGui::IsMouseHoveringRect({ 0, 0 }, { setting_bar_posX, ImGui::GetWindowWidth() }))
                {
                    is_settings_open = false;
                    setting_bar_posX = io->DisplaySize.x - 40;
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
        ImGui::End();
	}
}