#ifdef TOAD_LOADER
#include "toad.h"
#include "application.h"
#include "inject/injector.h"
#endif

#include <string>

std::thread init_thread;

namespace toad::ui
{

#ifdef TOAD_LOADER
    // ui when injected
    void ui_main(const ImGuiIO* io)
    {
        if (static bool once = false; !once)
        {
            if (init_thread.joinable())
                init_thread.join();
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
            ImGui::SetNextWindowSize(io->DisplaySize);
            once = true;
        }

        UI(io);
    }

    // for the flickering of the possible minecraft windows
    std::vector<window> shownWindowList = {};

    // ui when not injected 
    void ui_init(const ImGuiIO* io)
    {
        if (static bool once = false; !once)
        {
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
            ImGui::SetNextWindowSize(io->DisplaySize);
            once = true;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        ImGui::Begin("select minecraft", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
        {
            center_textX(ImVec4(0.3f, 0.3f, 0.3f, 1), "please select the minecraft window");
            ImGui::BeginChild("mc windows", ImVec2(0, 0), true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
            {
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
                        if (shownWindowList.size() != g_mc_window_list.size())
                            shownWindowList = g_mc_window_list;

                        count = false;
                    }
                }
                if (shownWindowList.size() != g_mc_window_list.size() && !count)
                {
                    timer = 0;
                    count = true;
                }

                if (shownWindowList.empty())
                {
                    center_text_multi(ImVec4(0.2f, 0.2f, 0.2f, 1), "make sure minecraft is opened. \n Supported clients: Lunar Client (1.8.9 & 1.7.10)");
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

                            g_curr_client = get_client_type(window.title);
                            std::cout << "currclient type: " << (int)g_curr_client << std::endl;
							if (init_thread.joinable())
								init_thread.join();

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

									g_injected_window = window;

									loading = false;
								});
                        }
                    }
                }
                if (loading)
                {
                    std::string state = "state: " + inject_status;
                    show_message_box("Injecting", state.c_str(), loading, false);
                    ImGui::SetCursorPos({ get_middle_point().x - 10, get_middle_point().y - 10 });
                    load_spinner("##injectingSpinner", 10, 3, IM_COL32(100, 100, 100, 255));
                }
                else if (failed_shared_mem)
                {
                    if (init_thread.joinable())
                        init_thread.join();
                    loading = false;
                    show_message_box("failed to initialize", "failed setting up ipc", failed_shared_mem, true, mboxType::ERR);
                }
                else if (failed_inject)
                {
                    if (init_thread.joinable())
                        init_thread.join();
                    loading = false;
                    show_message_box("failed to inject", inject_status.c_str(), failed_inject, true, mboxType::ERR);
                }
            }
            ImGui::EndChild(); // mc windows
        }
        ImGui::End(); // main window

        ImGui::PopStyleVar();
    }

#endif

    void clicker_rand_editor(bool* enabled)
    {
        if (static bool first = false; !first)
        {
            ImGui::SetNextWindowSize({ 300,300 });

            first = true;
        }

        ImGui::Begin("clicker rand edit", enabled, ImGuiWindowFlags_NoSavedSettings);
        {
            const size_t n_inconsistencies = left_clicker::rand.inconsistencies.size();
            const size_t n_inconsistencies2 = left_clicker::rand.inconsistencies2.size();
            const size_t n_boosts = left_clicker::rand.boosts.size();

            // prevent closing when adding/removing nodes
            static bool open_boost_node = false;
            static bool open_inconsistencies_node = false;
            static bool open_inconsistencies2_node = false;

            // stores the index
            static std::queue<int> remove_inconsistency_queue = {};
            static std::queue<int> remove_inconsistency2_queue = {};
            static std::queue<int> remove_boost_queue = {};

            if (open_inconsistencies_node)
            {
                ImGui::SetNextItemOpen(true);
                open_inconsistencies_node = false;
            }
            if (ImGui::TreeNode(("Inconsistencies (mouse down) " + std::to_string(n_inconsistencies)).c_str()))
            {
                for (size_t i = 0; i < n_inconsistencies; i++)
                {
                    ImGui::PushID((int)i);
                    if (ImGui::Button("X"))
                    {
                        remove_inconsistency_queue.push((int)i);
                    }

                    ImGui::SameLine();

                    auto& in = left_clicker::rand.inconsistencies[i];

                    in.min_amount_ms = std::clamp(in.min_amount_ms, -left_clicker::rand.min_delay + 1.0f, in.max_amount_ms);
                    in.max_amount_ms = std::clamp(in.max_amount_ms, in.min_amount_ms, 500.f);

                    if (ImGui::TreeNode(("inconsistency" + std::to_string(i)).c_str(), "inconsistency (%.1f, %.1f)", in.min_amount_ms, in.max_amount_ms))
                    {
                        ImGui::SliderFloat("min delay", &in.min_amount_ms, -left_clicker::rand.min_delay + 1.0f, 500.f, "%.3fms");
                        ImGui::SliderFloat("max delay", &in.max_amount_ms, -left_clicker::rand.min_delay + 1.0f, 500.f, "%.3fms");

                        ImGui::SliderInt("chance", &in.chance, 0, 100, "%d%%");
                        ImGui::SliderInt("frequency", &in.frequency, 1, 150, "every %d clicks");

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                }

                while (!remove_inconsistency_queue.empty())
                {
                    const auto index = remove_inconsistency_queue.front();
                    left_clicker::rand.inconsistencies.erase(left_clicker::rand.inconsistencies.begin() + index);
                    remove_inconsistency_queue.pop();
                    open_inconsistencies_node = true;
                }

                if (ImGui::Button("+"))
                {
                    left_clicker::rand.inconsistencies.emplace_back(0, 0, 50, 20);
                    open_inconsistencies_node = true;
                }
                ImGui::TreePop();
            }

            if (open_inconsistencies2_node)
            {
                ImGui::SetNextItemOpen(true);
                open_inconsistencies2_node = false;
            }
            if (ImGui::TreeNode(("Inconsistencies (mouse up) " + std::to_string(n_inconsistencies2)).c_str()))
            {
                for (size_t i = 0; i < n_inconsistencies2; i++)
                {
                    ImGui::PushID((int)i);
                    if (ImGui::Button("X"))
                    {
                        remove_inconsistency2_queue.push((int)i);
                    }

                    ImGui::SameLine();

                    auto& in = left_clicker::rand.inconsistencies2[i];

                    in.min_amount_ms = std::clamp(in.min_amount_ms, -left_clicker::rand.min_delay + 1.0f, in.max_amount_ms);
                    in.max_amount_ms = std::clamp(in.max_amount_ms, in.min_amount_ms, 500.f);

                    if (ImGui::TreeNode(("inconsistency" + std::to_string(i)).c_str(), "inconsistency (%.1f, %.1f)", in.min_amount_ms, in.max_amount_ms))
                    {
                        ImGui::SliderFloat("min delay", &in.min_amount_ms, -left_clicker::rand.min_delay + 1.0f, 500.f, "%.3fms");
                        ImGui::SliderFloat("max delay", &in.max_amount_ms, -left_clicker::rand.min_delay + 1.0f, 500.f, "%.3fms");

                        ImGui::SliderInt("chance", &in.chance, 0, 100, "%d%%");
                        ImGui::SliderInt("frequency", &in.frequency, 1, 150, "every %d clicks");

                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                while (!remove_inconsistency2_queue.empty())
                {
                    const auto index = remove_inconsistency2_queue.front();
                    left_clicker::rand.inconsistencies2.erase(left_clicker::rand.inconsistencies2.begin() + index);
                    remove_inconsistency2_queue.pop();
                    open_inconsistencies2_node = true;
                }

                if (ImGui::Button("+"))
                {
                    left_clicker::rand.inconsistencies2.emplace_back(0, 0, 50, 20);
                    open_inconsistencies2_node = true;
                }
                ImGui::TreePop();
            }

            if (open_boost_node)
            {
                ImGui::SetNextItemOpen(true);
                open_boost_node = false;
            }
            if (ImGui::TreeNode(("Boosts/Drops " + std::to_string(n_boosts)).c_str()))
            {
                for (size_t i = 0; i < n_boosts; i++)
                {
                    ImGui::PushID((int)i);
                    if (ImGui::Button("X"))
                    {
                        remove_boost_queue.push((int)i);
                    }

                    ImGui::SameLine();

                    auto& b = left_clicker::rand.boosts[i];

                    b.freq_min = std::clamp(b.freq_min, 5, b.freq_max);
                    b.freq_max = std::clamp(b.freq_max, b.freq_min, 200);

                    const char* name = b.amount_ms < 0 ? "drop" : "boost";
                    if (ImGui::TreeNode((std::to_string(b.id)).c_str(), "%s (%.1f)", name, b.amount_ms))
                    {
                        ImGui::SliderFloat("amount", &b.amount_ms, -100.f, 100.f, "%.3f ms");
                        ImGui::SliderInt("duration", &b.duration, 5, 100, "%d clicks");
                        ImGui::SliderInt("transition duration", &b.transition_duration, 5, 100, "%d clicks");
                        ImGui::SliderInt("freq min", &b.freq_min, 5, 200, "%d%%");
                        ImGui::SliderInt("freq max", &b.freq_max, 5, 200, "%d%%");

                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                while (!remove_boost_queue.empty())
                {
                    const auto index = remove_boost_queue.front();
                    left_clicker::rand.boosts.erase(left_clicker::rand.boosts.begin() + index);
                    remove_boost_queue.pop();
                    open_boost_node = true;
                }

                if (ImGui::Button("+"))
                {
                    left_clicker::rand.boosts.emplace_back(0.5f, 5, 50, 20, 50, n_boosts);
                    open_boost_node = true;
                }
                ImGui::TreePop();
            }
            if (ImGui::Button("Update Rand"))
            {
                visual_clicker.SetRand(left_clicker::rand);
                left_clicker::update_rand_flag = true;
            }
            if (left_clicker::update_rand_flag)
            {
                ImGui::SameLine();
                load_spinner("rand", 10, 2, IM_COL32_WHITE);
            }

            ImGui::End();
        }
    }

    void clicker_rand_visualizer(bool* enabled)
    {
        ImGui::Begin("clicker rand visualize", enabled, ImGuiWindowFlags_NoSavedSettings);
        {
            static bool
                show_rand = false,
                show_graph = false,
                show_playback = false;

            visual_clicker.d_time = ImGui::GetIO().DeltaTime;
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Extra"))
                {
                    ImGui::MenuItem("Show Graph", nullptr, &show_graph);
                    ImGui::MenuItem("Show Playback", nullptr, &show_playback);
                    ImGui::MenuItem("Show Rand Status", nullptr, &show_rand);

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            bool is_playing = visual_clicker.IsStarted();
            if (ImGui::Button(is_playing ? "pause" : "play"))
            {
                is_playing = !is_playing;
                is_playing ? visual_clicker.Start() : visual_clicker.Stop();
            }

            ImGui::Text("CPS: %d", visual_clicker.GetCPS());

            const auto& rand = visual_clicker.GetRand();
            ImGui::Text("range(%f - %f) | delay: %f", rand.edited_min, rand.edited_max, rand.delay);
            ImGui::Text("inconsistency delay: %f", rand.inconsistency_delay);

            /* static bool show_cps_bounds = false;
             ImGui::Checkbox("show cps bounds", &show_cps_bounds);

             if (show_cps_bounds)
             {
                 const auto draw = ImGui::GetWindowDrawList();
                 for (const auto& b : rand.boosts)
                 {
                     b.amount_ms* b.transition_duration;
                 }
             }*/

            ImGui::End();
        }
    }

    static ImGui::FileBrowser espFontDialog;

    void esp_visualizer(bool* enabled)
    {
        static bool once = false;
        if (!once)
        {
            espFontDialog.SetTitle("select a TrueType Font");
            espFontDialog.SetTypeFilters({ ".ttf" });

            ImGui::SetNextWindowSize({ 400, 700 });
            once = true;
        }

        ImGui::Begin("ESP box settings", enabled, ImGuiWindowFlags_NoSavedSettings);

        const auto window_size = ImGui::GetWindowSize();
        constexpr float window_spacing = 15;

        // for visualization
        static int visualize_health = 20;

        const static auto io = &ImGui::GetIO();
        static ImFont* preview_font = io->Fonts->Fonts[0];

        ImGui::BeginChild("esp box visual", { window_size.x / 2 - window_spacing, window_size.y - window_spacing - 20 }, true);
        {
            const auto window_pos = ImGui::GetWindowPos();

            const auto child_size = ImGui::GetWindowSize();

            ImVec2 min = { window_pos.x + window_spacing + 20 , window_pos.y + window_spacing + 20 };
            ImVec2 max = { window_pos.x + child_size.x - window_spacing - 20,  window_pos.y + child_size.y - window_spacing - 20 };

            // esp box
            ImGui::GetWindowDrawList()->AddRectFilled({ min.x - 1, min.y - 1 }, { max.x + 1, max.y + 1 }, ImGui::GetColorU32({ esp::fill_col[0], esp::fill_col[1], esp::fill_col[2], esp::fill_col[3] }));

            ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::GetColorU32({ esp::line_col[0], esp::line_col[1], esp::line_col[2], esp::line_col[3] }));

            if (esp::show_border)
            {
                ImGui::GetWindowDrawList()->AddRect({ min.x - 1, min.y - 1 }, { max.x + 1, max.y + 1 }, IM_COL32_BLACK);
                ImGui::GetWindowDrawList()->AddRect({ min.x + 1, min.y + 1 }, { max.x - 1, max.y - 1 }, IM_COL32_BLACK);
            }

            char text[30] = {};

            if (esp::show_name)
            {
                strncat_s(text, "player", strlen("player"));
            }
            if (esp::show_distance)
            {
                strncat_s(text, " [5.0]", strlen(" [5.0]"));
            }

            // draw our text
            const auto halfsizex = preview_font->CalcTextSizeA(esp::text_size, 500, 0, text).x / 2;
            const auto halfsizey = preview_font->CalcTextSizeA(esp::text_size, 500, 0, text).y / 2;

            const auto infoPosX = (min.x + max.x) / 2;

            if (esp::show_txt_bg)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(
                    { infoPosX - halfsizex - 5, min.y - 5 - halfsizey * 2 },
                    { infoPosX + halfsizex + 5, min.y - 5 },
                    ImGui::GetColorU32({ esp::text_bg_col[0], esp::text_bg_col[1], esp::text_bg_col[2], esp::text_bg_col[3] }));
            }

            const auto text_col_imu32 = ImGui::GetColorU32({ esp::text_col[0], esp::text_col[1], esp::text_col[2], esp::text_col[3] });

            // text position (positioned inside the background box) 
            auto pos_y_text = std::lerp(min.y - 5 - halfsizey * 2, min.y - 5, 0.9f) - esp::text_size;

            if (esp::text_shadow)
            {
                ImGui::GetWindowDrawList()->AddText(preview_font, esp::text_size, { infoPosX - halfsizex - 1, pos_y_text - 1 }, IM_COL32_BLACK, text);
                ImGui::GetWindowDrawList()->AddText(preview_font, esp::text_size, { infoPosX - halfsizex + 1, pos_y_text + 1 }, IM_COL32_BLACK, text);
            }

            ImGui::GetWindowDrawList()->AddText(preview_font, esp::text_size, { infoPosX - halfsizex, pos_y_text }, text_col_imu32, text);

            if (esp::show_health)
            {
                float t = (float)visualize_health / 20.f;
                ImVec4 col = { 0, 0, 0, 1 };
                if (t > 0.5f)
                {
                    // to yellow
                    col.x = std::lerp(1.f, 0.f, t / 0.5f - 1);
                    col.y = 1;
                }
                else
                {
                    // to red
                    col.x = 1;
                    col.y = std::lerp(0.f, 1.f, t / 0.5f);
                }

                if (esp::show_border)
                {
                    ImGui::GetWindowDrawList()->AddRectFilled({ max.x + 3, std::lerp(max.y, min.y - 1, t) }, { max.x + 7, max.y + 1 }, IM_COL32_BLACK);
                }

                // health bar
                // top: green -> middle: yellow -> bottom: red
                ImGui::GetWindowDrawList()->AddRectFilled({ max.x + 4, std::lerp(max.y, min.y, t) }, { max.x + 6, max.y }, ImGui::GetColorU32(col));
            }
            ImGui::EndChild();
        }

        ImGui::SameLine();

        ImGui::BeginChild("esp box properties", { window_size.x / 2 - window_spacing, window_size.y - window_spacing - 20 }, true);
        {
            constexpr auto color_edit_flags = ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaBar;

            if (ImGui::TreeNode("Player Properties"))
            {
                ImGui::Checkbox("show name", &esp::show_name);
                ImGui::Checkbox("show distance", &esp::show_distance);
                ImGui::Checkbox("show health", &esp::show_health);
                ImGui::Checkbox("show sneaking", &esp::show_sneaking);
                ImGui::SliderInt("Health", &visualize_health, 0, 20, "%d HP");

                ImGui::Spacing();

                ImGui::Checkbox("show background", &esp::show_txt_bg);
                ImGui::ColorEdit4("background color", esp::text_bg_col, color_edit_flags);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Box Properties"))
            {
                ImGui::Checkbox("Border", &esp::show_border);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("text/font"))
            {
                ImGui::ColorEdit4("text color", esp::text_col, color_edit_flags);
                ImGui::Checkbox("text shadow", &esp::text_shadow);
                ImGui::SliderFloat("text size", &esp::text_size, 10, 30, "%.1f");

                static std::string name = "Default";
                static std::string path = esp::font_path == "Default" ? ".." : esp::font_path.substr(0, esp::font_path.find_last_of("/\\") + 1);

                ImGui::Text("Font: %s", name.c_str());
                ImGui::Text("Path: %s", path.c_str());

                if (ImGui::Button("..."))
                {
                    espFontDialog.Open();
                }

                if (ImGui::Button("Set Default Font"))
                {
                    name = "Default";
                    path = "..";
                    esp::font_path = "Default";
                    esp::update_font_flag = true;
#ifdef TOAD_LOADER
                    Application::Get()->GetWindow()->AddFontTTF(esp::font_path);
#endif
                }

                if (espFontDialog.HasSelected())
                {
                    const auto selected = espFontDialog.GetSelected();
                    esp::font_path = selected.string();

                    path = esp::font_path.substr(0, esp::font_path.find_last_of("/\\") + 1);
                    name = selected.filename().string();

                    espFontDialog.ClearSelected();
                    esp::update_font_flag = true;
#ifdef TOAD_LOADER
                    Application::Get()->GetWindow()->AddFontTTF(esp::font_path);
#endif
                }

#ifdef TOAD_LOADER
                // update preview font
                if (esp::update_font_flag)
                {
                    if (Application::Get()->GetWindow()->IsFontUpdated())
                    {
                        // get newly added font
                        preview_font = io->Fonts->Fonts.back();
                    }
                }

                if (esp::update_font_flag && !Application::Get()->GetWindow()->IsFontUpdated())
                {
                    load_spinner("update font spinner", 10, 2, IM_COL32_WHITE);
                }
#else
                if (esp::update_font_flag)
                {
                    load_spinner("update font spinner", 10, 2, IM_COL32_WHITE);
                }
#endif


                ImGui::TreePop();
            }

        }
        ImGui::EndChild();

        ImGui::End();

        espFontDialog.Display();
    }

    void chest_stealer_slotpos_setter(bool* enabled)
    {
        const static ImGuiID popup_cheststealer_slot = ImHashStr("POPUP_CHESTSTEALER_SLOT");
        ImGui::Begin("Chest Stealer Slot Settings", enabled, ImGuiWindowFlags_NoSavedSettings);
        {
			ImGui::Checkbox("show slot positions", &chest_stealer::show_slot_positions);
            if (ImGui::Button("Add"))
            {
                chest_stealer::slot_info.emplace_back();
            }

            uint32_t id = 0;
            static uint32_t selected_slot_for_popup = 0;

            const auto open_popup = [&]
                {
                    ImGui::PushOverrideID(popup_cheststealer_slot);
                    selected_slot_for_popup = id;
                    ImGui::OpenPopup("POPUP_CHESTSTEALER_SLOT");
                    ImGui::PopID();
                };
            for (auto& setting : chest_stealer::slot_info)
            {
                ImGui::PushID(id);
                if (ImGui::TreeNode("###item", "%dx%d", setting.res_x, setting.res_y))
                {
					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
					{
                        open_popup();
					}

                    ImGui::DragInt("space x", &setting.space_x);
                    ImGui::DragInt("space y", &setting.space_y);
                    ImGui::DragInt("pos x", &setting.begin_x);
                    ImGui::DragInt("pos y", &setting.begin_y);

                    bool bind_to_res = setting.res_x != -1 || setting.res_y != -1;
                    if (ImGui::Checkbox("bind to resolution", &bind_to_res))
                    {
                        if (bind_to_res)
                        {
#ifdef TOAD_LOADER
                            // #TODO: thes give weird values when in fullscreen.
                            RECT r;
                            GetWindowRect(toad::g_injected_window.hwnd, &r);

                            setting.res_x = r.right - r.left - 16;
                            setting.res_y = r.bottom - r.top - 39;
#else
                            setting.res_x = toadll::g_screen_width;
                            setting.res_y = toadll::g_screen_height;
#endif 
                        }
                        else
                        {
                            setting.res_x = -1;
                            setting.res_y = -1;
                        }
                    }
                    ImGui::BeginDisabled(setting.res_x == -1 || setting.res_y == -1);

                    ImGui::DragInt("res x", &setting.res_x);
                    ImGui::DragInt("res y", &setting.res_y);

                    ImGui::EndDisabled();

                    ImGui::TreePop();
                }

                if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                {
                    open_popup();
                }

                id++;
                ImGui::PopID();
            }

            ImGui::PushOverrideID(popup_cheststealer_slot);
            if (ImGui::BeginPopup("POPUP_CHESTSTEALER_SLOT"))
            {
                if (ImGui::MenuItem("Delete"))
                {
                    chest_stealer::slot_info.erase(chest_stealer::slot_info.begin() + selected_slot_for_popup);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
                
        }
		ImGui::End();
    }

}