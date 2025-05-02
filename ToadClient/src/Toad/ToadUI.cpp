#include "pch.h"

#include "Inject/injector.h"
#include "implot/implot.h"
#include "Utils/ui_helpers.h"
#include "Toad/ToadUI.h"
#include "Toad/toad.h"
#include "Toad/config.h"
#include "Utils/block_map.h"

#ifdef TOAD_LOADER
#include "Application/application.h"
#endif 

#include <string>

namespace toad
{
namespace UI
{
    void clicker_rand_editor(bool* enabled)
    {
        if (static bool first = false; !first)
        {
            ImGui::SetNextWindowSize({ 300,300 });

            first = true;
        }

        ImGui::Begin("clicker rand edit", enabled, ImGuiWindowFlags_NoSavedSettings);
        {
            const size_t n_inconsistencies = settings.lc_rand.inconsistencies.size();
            const size_t n_inconsistencies2 = settings.lc_rand.inconsistencies2.size();
            const size_t n_boosts = settings.lc_rand.boosts.size();

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

                    auto& in = settings.lc_rand.inconsistencies[i];

                    in.min_amount_ms = std::clamp(in.min_amount_ms, -settings.lc_rand.min_delay + 1.0f, in.max_amount_ms);
                    in.max_amount_ms = std::clamp(in.max_amount_ms, in.min_amount_ms, 500.f);

                    if (ImGui::TreeNode(("inconsistency" + std::to_string(i)).c_str(), "inconsistency (%.1f, %.1f)", in.min_amount_ms, in.max_amount_ms))
                    {
                        ImGui::SliderFloat("min delay", &in.min_amount_ms, -settings.lc_rand.min_delay + 1.0f, 500.f, "%.3fms");
                        ImGui::SliderFloat("max delay", &in.max_amount_ms, -settings.lc_rand.min_delay + 1.0f, 500.f, "%.3fms");

                        ImGui::SliderInt("chance", &in.chance, 0, 100, "%d%%");
                        ImGui::SliderInt("frequency", &in.frequency, 1, 150, "every %d clicks");

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                }

                while (!remove_inconsistency_queue.empty())
                {
                    const auto index = remove_inconsistency_queue.front();
                    settings.lc_rand.inconsistencies.erase(settings.lc_rand.inconsistencies.begin() + index);
                    remove_inconsistency_queue.pop();
                    open_inconsistencies_node = true;
                }

                if (ImGui::Button("+"))
                {
                    settings.lc_rand.inconsistencies.emplace_back(0, 0, 50, 20);
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

                    auto& in = settings.lc_rand.inconsistencies2[i];

                    in.min_amount_ms = std::clamp(in.min_amount_ms, -settings.lc_rand.min_delay + 1.0f, in.max_amount_ms);
                    in.max_amount_ms = std::clamp(in.max_amount_ms, in.min_amount_ms, 500.f);

                    if (ImGui::TreeNode(("inconsistency" + std::to_string(i)).c_str(), "inconsistency (%.1f, %.1f)", in.min_amount_ms, in.max_amount_ms))
                    {
                        ImGui::SliderFloat("min delay", &in.min_amount_ms, -settings.lc_rand.min_delay + 1.0f, 500.f, "%.3fms");
                        ImGui::SliderFloat("max delay", &in.max_amount_ms, -settings.lc_rand.min_delay + 1.0f, 500.f, "%.3fms");

                        ImGui::SliderInt("chance", &in.chance, 0, 100, "%d%%");
                        ImGui::SliderInt("frequency", &in.frequency, 1, 150, "every %d clicks");

                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                while (!remove_inconsistency2_queue.empty())
                {
                    const auto index = remove_inconsistency2_queue.front();
                    settings.lc_rand.inconsistencies2.erase(settings.lc_rand.inconsistencies2.begin() + index);
                    remove_inconsistency2_queue.pop();
                    open_inconsistencies2_node = true;
                }

                if (ImGui::Button("+"))
                {
                    settings.lc_rand.inconsistencies2.emplace_back(0, 0, 50, 20);
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

                    auto& b = settings.lc_rand.boosts[i];

                    b.freq_min = std::clamp(b.freq_min, 5, b.freq_max);
                    b.freq_max = std::clamp(b.freq_max, b.freq_min, 200);

                    const char* name = b.amount_ms < 0 ? "drop" : "boost";
                    if (ImGui::TreeNode((std::to_string(b.id)).c_str(), "%s (%.1f)", name, b.amount_ms))
                    {
                        ImGui::SliderFloat("amount", &b.amount_ms, -100.f, 100.f, "%.3f ms");
                        ImGui::SliderInt("duration", &b.duration, 5, 100, "%d clicks");
                        ImGui::SliderInt("transition duration", &b.transition_duration, 5, 100, "%d clicks");
                        ImGui::SliderInt("freq min", &b.freq_min, 5, 200, "%d clicks");
                        ImGui::SliderInt("freq max", &b.freq_max, 5, 200, "%d clicks");

                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                while (!remove_boost_queue.empty())
                {
                    const auto index = remove_boost_queue.front();
                    settings.lc_rand.boosts.erase(settings.lc_rand.boosts.begin() + index);
                    remove_boost_queue.pop();
                    open_boost_node = true;
                }

                if (ImGui::Button("+"))
                {
                    settings.lc_rand.boosts.emplace_back(0.5f, 5, 50, 20, 50, n_boosts);
                    open_boost_node = true;
                }
                ImGui::TreePop();
            }
            if (ImGui::Button("Update Rand"))
            {
                visual_clicker.SetRand(settings.lc_rand);
                settings.lc_update_rand_flag = true;
            }
            if (settings.lc_update_rand_flag)
            {
                ImGui::SameLine();
                load_spinner("rand", 10, 2, IM_COL32_WHITE);
            }

            ImGui::End();
        }
    }

    void clicker_rand_visualizer(bool* enabled)
    {
        ImGui::Begin("clicker rand visualize", enabled, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar);
        {
            static bool show_rand = false;
			static bool show_graph = false;
			static bool show_playback = false;

			static std::map<float, uint32_t> cps_counter{};
            const auto click_callback = []
                {
                    float cps = std::round(visual_clicker.GetCPS() * 100) / 100;
                    if (cps_counter.contains(cps))
                        cps_counter[cps]++;
                    else 
						cps_counter[cps] = 1;
                };

            visual_clicker.d_time = ImGui::GetIO().DeltaTime;

            std::filesystem::path opened_file;
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open"))
                    {
                        std::vector<std::string> file_types {".txt"};
                        opened_file = FileDialogGetFile(get_exe_path(), file_types);
                    }
                    ImGui::EndMenu();
				}

                if (ImGui::BeginMenu("Extra"))
				{
                    if (ImGui::MenuItem("Show Graph", nullptr, &show_graph))
                    {
                        cps_counter.clear();
                        visual_clicker.SetClickCallback(click_callback);
                    }

					ImGui::MenuItem("Show Playback", nullptr, &show_playback);
					ImGui::MenuItem("Show Rand Status", nullptr, &show_rand);

					ImGui::EndMenu();
				}

                ImGui::EndMenuBar();
            }

            if (!opened_file.empty())
            {

                opened_file.clear();
            }

            bool is_playing = visual_clicker.IsStarted();
            if (ImGui::Button(is_playing ? "pause" : "play"))
            {
                is_playing = !is_playing;
                is_playing ? visual_clicker.Start() : visual_clicker.Stop();
            }

            ImGui::Text("CPS: %.2f", visual_clicker.GetCPS());

            const Randomization& rand = visual_clicker.GetRand();
            ImGui::Text("range(%f - %f) | delay: %f", rand.edited_min, rand.edited_max, rand.delay);
            ImGui::Text("inconsistency delay: %f", rand.inconsistency_delay);
            
            if (show_graph)
            {
                if (!cps_counter.empty())
                {
					std::vector<float> x{};
					std::vector<float> y{};	

                    for (const auto& [cps, count] : cps_counter)
                    {
                        if (count == 0)
                            continue;

                        x.emplace_back(cps);
                        y.emplace_back((float)count);
                    }

                    static ImPlotLineFlags plot_flags = ImPlotLineFlags_None;

                    if (ImPlot::BeginPlot("distribution", { -1, 0 }))
                    {
                        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);

						float range = *std::max_element(x.begin(), x.end()) - *std::min_element(x.begin(), x.end());
						ImPlot::SetupAxisFormat(ImAxis_X1, "%.2f");
						ImPlot::PlotLine("dis", x.data(), y.data(), x.size(), plot_flags);

                        ImPlot::EndPlot();
                    }
                    //ImGui::PlotLines("distribution", x.data(), (int)x.size(), 0, nullptr, FLT_MIN, FLT_MAX, { 0, 70.f });

                    if (ImGui::Button("Clear"))
                        cps_counter.clear();
                }
            }

            ImGui::End();
        }
    }

    void esp_visualizer(bool* enabled)
    {
        static bool once = false;
        if (!once)
        {
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
            ImGui::GetWindowDrawList()->AddRectFilled({ min.x - 1, min.y - 1 }, { max.x + 1, max.y + 1 }, ImGui::GetColorU32({ settings.esp_fill_col[0], settings.esp_fill_col[1], settings.esp_fill_col[2], settings.esp_fill_col[3] }));

            ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::GetColorU32({ settings.esp_line_col[0], settings.esp_line_col[1], settings.esp_line_col[2], settings.esp_line_col[3] }));

            if (settings.esp_show_border)
            {
                ImGui::GetWindowDrawList()->AddRect({ min.x - 1, min.y - 1 }, { max.x + 1, max.y + 1 }, IM_COL32_BLACK);
                ImGui::GetWindowDrawList()->AddRect({ min.x + 1, min.y + 1 }, { max.x - 1, max.y - 1 }, IM_COL32_BLACK);
            }

            char text[30] = {};

            if (settings.esp_show_name)
            {
                strncat_s(text, "player", strlen("player"));
            }
            if (settings.esp_show_distance)
            {
                strncat_s(text, " [5.0]", strlen(" [5.0]"));
            }

            // draw our text
            const auto halfsizex = preview_font->CalcTextSizeA(settings.esp_text_size, 500, 0, text).x / 2;
            const auto halfsizey = preview_font->CalcTextSizeA(settings.esp_text_size, 500, 0, text).y / 2;

            const auto infoPosX = (min.x + max.x) / 2;

            if (settings.esp_show_txt_bg)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(
                    { infoPosX - halfsizex - 5, min.y - 5 - halfsizey * 2 },
                    { infoPosX + halfsizex + 5, min.y - 5 },
                    ImGui::GetColorU32({ settings.esp_text_bg_col[0], settings.esp_text_bg_col[1], settings.esp_text_bg_col[2], settings.esp_text_bg_col[3] }));
            }

            const auto text_col_imu32 = ImGui::GetColorU32({ settings.esp_text_col[0], settings.esp_text_col[1], settings.esp_text_col[2], settings.esp_text_col[3] });

            // text position (positioned inside the background box) 
            auto pos_y_text = std::lerp(min.y - 5 - halfsizey * 2, min.y - 5, 0.9f) - settings.esp_text_size;

            if (settings.esp_text_shadow)
            {
                ImGui::GetWindowDrawList()->AddText(preview_font, settings.esp_text_size, { infoPosX - halfsizex - 1, pos_y_text - 1 }, IM_COL32_BLACK, text);
                ImGui::GetWindowDrawList()->AddText(preview_font, settings.esp_text_size, { infoPosX - halfsizex + 1, pos_y_text + 1 }, IM_COL32_BLACK, text);
            }

            ImGui::GetWindowDrawList()->AddText(preview_font, settings.esp_text_size, { infoPosX - halfsizex, pos_y_text }, text_col_imu32, text);

            if (settings.esp_show_health)
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

                if (settings.esp_show_border)
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
                ImGui::Checkbox("show name", &settings.esp_show_name);
                ImGui::Checkbox("show distance", &settings.esp_show_distance);
                ImGui::Checkbox("show health", &settings.esp_show_health);
                ImGui::Checkbox("show sneaking", &settings.esp_show_sneaking);
                ImGui::SliderInt("Health", &visualize_health, 0, 20, "%d HP");

                ImGui::Spacing();

                ImGui::Checkbox("show background", &settings.esp_show_txt_bg);
                ImGui::ColorEdit4("background color", settings.esp_text_bg_col, color_edit_flags);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Box Properties"))
            {
                ImGui::Checkbox("Border", &settings.esp_show_border);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("text/font"))
            {
                ImGui::ColorEdit4("text color", settings.esp_text_col, color_edit_flags);
                ImGui::Checkbox("text shadow", &settings.esp_text_shadow);
                ImGui::SliderFloat("text size", &settings.esp_text_size, 10, 30, "%.1f");

                static std::string name = "Default";
                static std::string path = settings.esp_font_path == "Default" ? ".." : settings.esp_font_path.substr(0, settings.esp_font_path.find_last_of("/\\") + 1);

                ImGui::Text("Font: %s", name.c_str());
                ImGui::Text("Path: %s", path.c_str());

                std::filesystem::path selected_font_file = "";
                if (ImGui::Button("..."))
                {
                    std::vector<std::string> file_types = { ".ttf" };
                    selected_font_file = FileDialogGetFile(get_exe_path().string(), file_types);
                }

                if (ImGui::Button("Set Default Font"))
                {
                    name = "Default";
                    path = "..";
                    settings.esp_font_path = "Default";
                    settings.esp_update_font_flag = true;
#ifdef TOAD_LOADER
                    Application::GetWindow().AddFontTTF(settings.esp_font_path);
#endif
                }

                if (!selected_font_file.empty())
                {
                    settings.esp_font_path = selected_font_file.string();

                    path = settings.esp_font_path.substr(0, settings.esp_font_path.find_last_of("/\\") + 1);
                    name = selected_font_file.filename().string();
                    selected_font_file.clear();

                    settings.esp_update_font_flag = true;
#ifdef TOAD_LOADER
                    Application::GetWindow().AddFontTTF(settings.esp_font_path);
#endif
                }

#ifdef TOAD_LOADER
                // update preview font
                if (settings.esp_update_font_flag)
                {
                    if (Application::GetWindow().IsFontUpdated())
                    {
                        // get newly added font
                        preview_font = io->Fonts->Fonts.back();
                    }
                }

                if (settings.esp_update_font_flag && !Application::GetWindow().IsFontUpdated())
                {
                    load_spinner("update font spinner", 10, 2, IM_COL32_WHITE);
                }
#else
                if (settings.esp_update_font_flag)
                {
                    load_spinner("update font spinner", 10, 2, IM_COL32_WHITE);
                }
#endif


                ImGui::TreePop();
            }

        }
        ImGui::EndChild();

        ImGui::End();
    }

    void chest_stealer_slotpos_setter(bool* enabled)
    {
        const static ImGuiID popup_cheststealer_slot = ImHashStr("POPUP_CHESTSTEALER_SLOT");
        ImGui::Begin("Chest Stealer Slot Settings", enabled, ImGuiWindowFlags_NoSavedSettings);
        {
			ImGui::Checkbox("show slot positions", &settings.cs_show_slot_positions);
            if (ImGui::Button("Add"))
            {
                settings.cs_slot_info.emplace_back();
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
            for (auto& setting : settings.cs_slot_info)
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
                            GetWindowRect(toad::get_injected_window()->hwnd, &r);

                            setting.res_x = r.right - r.left - 16;
                            setting.res_y = r.bottom - r.top - 39;
#else
                            setting.res_x = toad::g_screen_width;
                            setting.res_y = toad::g_screen_height;
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
                    settings.cs_slot_info.erase(settings.cs_slot_info.begin() + selected_slot_for_popup);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
                
        }
		ImGui::End();
    }

	void ToadUI(const ImGuiIO* io)
	{
		// ui settings
		// #TODO: implement
		static bool tooltips = false;

		// clicker extra option rand edit
		static bool clicker_rand_edit = false;
		static bool clicker_rand_visualize = false;

		// esp visuals visualization
		static bool esp_visuals_menu = false;

		static bool chest_stealer_slot_info_edit = false;

		// config
		static std::vector<ConfigFile> available_configs = {};

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
						available_configs = Config::GetAllConfigsInDirectory(settings.loader_path);
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
					if (checkbox_button("Left Clicker", ICON_FA_MOUSE, &settings.lc_enabled, &settings.lc_key))
						menu_lc = true;
					if (settings.g_curr_client == MC_CLIENT::Lunar_189)
					{
						ImGui::SameLine(0, 80);
						checkbox_button("No Hit Delay", ICON_FA_CLOCK, &settings.ncd_enabled);
					}
					if (checkbox_button("Right Clicker", ICON_FA_MOUSE, &settings.rc_enabled, &settings.rc_key))
						menu_rc = true;
					if (checkbox_button("Aim Assist", ICON_FA_CROSSHAIRS, &settings.aa_enabled, &settings.aa_key))
						menu_aa = true;
					if (checkbox_button("Velocity", ICON_FA_WIND, &settings.vel_enabled, &settings.vel_key))
						menu_vel = true;

					if (menu_lc)
						setting_menu("LeftClicker", menu_lc, []
							{
								if (ImGui::SliderFloat("min cps", &settings.lc_min_cps, 5, 20, "%.1fcps", ImGuiSliderFlags_NoInput))
								{
									if (settings.lc_min_cps > settings.lc_max_cps)
										settings.lc_max_cps = settings.lc_min_cps;

									// update rand delays
									settings.lc_rand.UpdateDelays(settings.lc_min_cps, settings.lc_max_cps);
									visual_clicker.SetRand(settings.lc_rand);
								}

								if (ImGui::SliderFloat("max cps", &settings.lc_max_cps, 5, 20, "%.1fcps", ImGuiSliderFlags_NoInput))
								{
									if (settings.lc_min_cps > settings.lc_max_cps)
										settings.lc_min_cps = settings.lc_max_cps;

									// update rand delays
									settings.lc_rand.UpdateDelays(settings.lc_min_cps, settings.lc_max_cps);
									visual_clicker.SetRand(settings.lc_rand);
								}

								ImGui::Checkbox("weapons only", &settings.lc_weapons_only);
								ImGui::Checkbox("break blocks", &settings.lc_break_blocks);
								ImGui::Checkbox("block hit", &settings.lc_block_hit);
								ImGui::Checkbox("smart cps", &settings.lc_targeting_affects_cps);
								ImGui::Checkbox("trade assist", &settings.lc_trade_assist);
								if (ImGui::BeginCombo("click check", clickCheckToCStrMap[settings.lc_click_check], ImGuiComboFlags_NoArrowButton))
								{
									for (const auto& [clickCheck, name] : clickCheckToCStrMap)
									{
										if (ImGui::Selectable(name, clickCheck == settings.lc_click_check))
											settings.lc_click_check = clickCheck;
									}
									ImGui::EndCombo();
								}
							}, &settings.lc_key, true,
							[] {
								if (ImGui::Checkbox("Edit Boosts & Drops", &clicker_rand_edit))
								{
									auto rand = visual_clicker.GetRand();
									rand.UpdateDelays(settings.lc_min_cps, settings.lc_max_cps);
									visual_clicker.SetRand(rand);
								}
								else if (ImGui::Checkbox("Visualize", &clicker_rand_visualize))
								{
									auto rand = visual_clicker.GetRand();
									rand.UpdateDelays(settings.lc_min_cps, settings.lc_max_cps);
									visual_clicker.SetRand(rand);
								}

								ImGui::Spacing();

								ImGui::Text("break blocks");
								ImGui::Separator();
								ImGui::SliderInt("start delay", &settings.lc_start_break_blocks_reaction, 30, 500, "%dms");
								ImGui::SliderInt("stop delay", &settings.lc_stop_break_blocks_reaction, 30, 500, "%dms");

								ImGui::Spacing();

								ImGui::Text("block hit");
								ImGui::Separator();
								ImGui::Checkbox("pause left click", &settings.lc_block_hit_stop_lclick);
								ImGui::SliderInt("block hit delay", &settings.lc_block_hit_ms, 10, 200);
								});

					else if (menu_rc)
						setting_menu("RightClicker", menu_rc, []
							{
								ImGui::SliderInt("cps", &settings.rc_cps, 1, 20, "%dcps");
								ImGui::Checkbox("blocks only", &settings.rc_blocks_only);
								ImGui::SliderInt("start delay", &settings.rc_start_delayms, 0, 200, "%dms");
								if (ImGui::BeginCombo("click check", clickCheckToCStrMap[settings.rc_click_check], ImGuiComboFlags_NoArrowButton))
								{
									for (const auto& [clickCheck, name] : clickCheckToCStrMap)
									{
										if (ImGui::Selectable(name, clickCheck == settings.rc_click_check))
											settings.rc_click_check = clickCheck;
									}
									ImGui::EndCombo();
								}
							}, &settings.rc_key);

					else if (menu_aa)
						setting_menu("Aim Assist", menu_aa, []
							{
								ImGui::SliderFloat("Speed", &settings.aa_speed, 0, 200);
								ImGui::SliderInt("Fov Check", &settings.aa_fov, 0, 360);
								ImGui::SliderFloat("Distance", &settings.aa_distance, 0, 10);
								ImGui::Checkbox("Horizontal Only", &settings.aa_horizontal_only);
								ImGui::Checkbox("Invisibles", &settings.aa_invisibles);
								ImGui::Checkbox("Always Aim", &settings.aa_always_aim);
								ImGui::Checkbox("Target Lock", &settings.aa_lock_aim);
								ImGui::Checkbox("Aim in target", &settings.aa_aim_at_closest_point);
								ImGui::Checkbox("Break Blocks", &settings.aa_break_blocks);
								if (ImGui::BeginCombo("target by", AATargetToCStrMap[settings.aa_target_mode]))
								{
									for (const auto& [aaMode, name] : AATargetToCStrMap)
									{
										if (ImGui::Selectable(name, aaMode == settings.aa_target_mode))
											settings.aa_target_mode = aaMode;
									}
									ImGui::EndCombo();
								}
							}, &settings.aa_key);

					else if (menu_vel)
						setting_menu("Velocity", menu_vel, []
							{
								ImGui::Checkbox("Use Jump Reset", &settings.vel_jump_reset);

								if (settings.vel_jump_reset)
								{
									ImGui::SliderInt("Press Chance", &settings.vel_jump_press_chance, 0, 100, "%d%%");
								}

								ImGui::Checkbox("Only when moving", &settings.vel_only_when_moving);
								ImGui::Checkbox("Only when clicking", &settings.vel_only_when_clicking);
								ImGui::Checkbox("Kite", &settings.vel_kite);
								if (!settings.vel_jump_reset)
								{
									ImGui::SliderFloat("Horizontal", &settings.vel_horizontal, 0, 100.f, "%.1f%%");
									ImGui::SliderFloat("Vertical", &settings.vel_vertical, 0.f, 100.f, "%.1f%%");
									ImGui::SliderInt("Chance", &settings.vel_chance, 0, 100, "%d%%");
									ImGui::SliderInt("Delay", &settings.vel_delay, 0, 10);
								}
							}, &settings.vel_key);
				}
				else if (tab == 1)
				{
					static bool is_Bridge = false;
					static bool is_Esp = false;
					static bool is_BlockEsp = false;
					static bool is_Blink = false;
					static bool is_ArrayList = false;
					static bool is_ChestStealer = false;
					if (checkbox_button("Bridge Assist", ICON_FA_CUBE, &settings.ba_enabled, &settings.ba_key)) is_Bridge = true;
					ImGui::SameLine(0, 80);
					if (checkbox_button("Array List", ICON_FA_BARS, &toad::settings.ui_show_array_list)) is_ArrayList = true;
					if (checkbox_button("ESP", ICON_FA_EYE, &settings.esp_enabled, &settings.esp_key)) is_Esp = true;
					ImGui::SameLine(0, 80);
					ImGui::BeginDisabled();
					if (checkbox_button("Chest Stealer", ICON_FA_BOX_OPEN, &settings.cs_enabled, &settings.cs_key)) is_ChestStealer = true;
					ImGui::EndDisabled();
					if (checkbox_button("Block ESP", ICON_FA_CUBES, &settings.besp_enabled, &settings.besp_key)) is_BlockEsp = true;
					if (checkbox_button("Blink", ICON_FA_GHOST, &settings.bl_enabled, &settings.bl_key)) is_Blink = true;

					if (is_Bridge)
					{
						setting_menu("Bridge Assist", is_Bridge, []
							{
								ImGui::SliderFloat("pitch check", &settings.ba_pitch_check, 1, 90);

								// zero and 1 means sneak at any edge
								ImGui::SliderInt("block height check", &settings.ba_block_check, 0, 10);

								ImGui::Checkbox("Only initiate when sneaking", &settings.ba_only_initiate_when_sneaking);
							}, &settings.ba_key);
					}

					else if (is_Esp)
					{
						constexpr auto color_edit_flags = ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaBar;

						setting_menu("ESP", is_Esp, []
							{
								ImGui::ColorEdit4("Outline Color", settings.esp_line_col, color_edit_flags);
								ImGui::ColorEdit4("Fill Color", settings.esp_fill_col, color_edit_flags);

								if (ImGui::BeginCombo("ESP Type", espModeToCStrMap[settings.esp_esp_mode], ImGuiComboFlags_NoArrowButton))
								{
									for (const auto& [espMode, name] : espModeToCStrMap)
									{
										if (ImGui::Selectable(name, espMode == settings.esp_esp_mode))
											settings.esp_esp_mode = espMode;
									}
									ImGui::EndCombo();
								}

								ImGui::Checkbox("open esp settings", &esp_visuals_menu);
							}, &settings.esp_key);
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
									if (!settings.besp_block_list.contains(blockIdInput))
									{
										settings.besp_block_list.insert({ blockIdInput, ImVec4{ 1, 1, 1, 0.3f } });
										ignoreSuggestions.insert(ignoreSuggestions.end(), NameOfBlockId[blockIdInput]);
										ZeroMemory(buf, 20);
									}
								}

								auto listPos = ImGui::GetCursorPos();
								static std::queue<int> removeQueue = {};

								ImGui::SetCursorPos(listPos);
								ImGui::BeginChild("esp block list", {}, true);

								for (auto& [id, col] : settings.besp_block_list)
								{
									ImGui::PushID(id);

									// Info
									ImGui::Text("%s | %d", NameOfBlockId[id].c_str(), id);

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
									settings.besp_block_list.erase(id);
									ignoreSuggestions.erase(NameOfBlockId[id]);
									removeQueue.pop();
								}

								ImGui::EndChild();

								ImGui::SetCursorPos(listPos);
								auto suggestions = get_filtered_suggestions(buf, NameOfBlockId, ignoreSuggestions);
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
							}, &settings.besp_key);
					}

					else if (is_Blink)
					{
						setting_menu("Blink", is_Blink, []
							{
								ImGui::InputInt("keycode", &settings.bl_hold_key);
								ImGui::InputFloat("max limit in seconds", &settings.bl_limit_seconds, 0, 0, "%.1f");
								ImGui::Checkbox("render trail", &settings.bl_show_trail);
								ImGui::Checkbox("pause incoming packets", &settings.bl_stop_rec_packets);
							}, &settings.bl_key);
					}

					else if (is_ArrayList)
					{
						setting_menu("Array List", is_ArrayList, []
							{
								// #TODO: add array list options
								center_text_multi({ 1,1,1,1 }, "WIP. \n customisation options will be \n added here later");
							});
					}

					else if (is_ChestStealer)
					{
						setting_menu("Chest Stealer", is_ChestStealer, []
							{
								ImGui::Text("steal button");
								ImGui::SameLine();
								keybind_button(settings.cs_steal_key);

								ImGui::Checkbox("set slot positions", &chest_stealer_slot_info_edit);
								ImGui::SliderInt("delay ms", &settings.cs_average_slowness_ms, 10, 200);

								static char buf[64];
								static char rename_buf[64];
								static int selected_item_index = -1;
								static bool rename_item = false;
								static ImGuiID cs_items_popup = ImHashStr("POPUP_CS_ITEMS");

								if (ImGui::InputText("item", buf, 64, ImGuiInputTextFlags_EnterReturnsTrue))
								{
									settings.cs_items_to_grab.emplace_back(buf);
								}
								ImGui::SameLine();
								ImGui::BeginDisabled(strlen(buf) == 0);
								if (ImGui::Button("Add"))
								{
									settings.cs_items_to_grab.emplace_back(buf);
								}
								ImGui::EndDisabled();

								ImGui::BeginChild("Items", { 0, 0 }, ImGuiChildFlags_Border);
								{
									for (size_t i = 0; i < settings.cs_items_to_grab.size(); i++)
									{
										std::string& item = settings.cs_items_to_grab[i];

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
										settings.cs_items_to_grab.erase(settings.cs_items_to_grab.begin() + selected_item_index);
									}
									if (ImGui::MenuItem("Rename"))
									{
										rename_item = true;
									}
									ImGui::EndPopup();
								}
								ImGui::PopID();

								center_text_multi({ 1, 1, 0, 0.5f }, "This feature is WIP. \n only works on small chests. \n Or not at all.");
							}, &settings.cs_key);
					}
				}
				else if (tab == 2)
				{
					static char searching_dir[MAX_PATH];
					static char config_name_buf[50] = {};

					static bool once = false;
					if (!once)
					{
						memcpy(searching_dir, settings.loader_path.c_str(), settings.loader_path.length());
						memcpy(config_name_buf, settings.loaded_config.c_str(), settings.loaded_config.length());
						once = true;
					}

					ImGui::BeginChild("config list", { ImGui::GetWindowSize().x / 2 - 5, 0 }, true);
					{
						ImGui::BeginChild("##available configs", { 0, 150 }, true);
						{
							const auto size_x = ImGui::CalcTextSize(searching_dir).x;
							ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x / 2 - size_x / 2);
							if (ImGui::Button(searching_dir))
							{
								Config::GetAllConfigsInDirectory(searching_dir);
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
										ZeroMemory(config_name_buf, sizeof(config_name_buf));
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
							available_configs = Config::GetAllConfigsInDirectory(searching_dir);
						}

						ImGui::BeginDisabled(strlen(config_name_buf) == 0);
						if (ImGui::Button("Load"))
						{
							settings.LoadConfig(searching_dir, config_name_buf);
						}

						if (ImGui::Button("Save"))
						{
							settings.SaveConfig(searching_dir, config_name_buf);
						}
						ImGui::EndDisabled();

						ImGui::InputText("name", config_name_buf, 50);

						ImGui::EndChild();
					}

					ImGui::SameLine(0, 5);

					ImGui::BeginChild("##config selection menu", { ImGui::GetContentRegionAvail().x - 30, 0 }, true);
					{
						if (ImGui::Button("Load from Clipboard"))
						{
							settings.LoadConfigFromClipBoard();
						}
						if (ImGui::Button("Save to Clipboard"))
						{
							settings.SaveConfigToClipBoard();
						}
						ImGui::EndChild();
					}

				}
			}
			ImGui::EndChild();

			// side extra settings bar

			auto window_right = ImVec2{ ImGui::GetContentRegionMax().x + 10,ImGui::GetContentRegionMax().y };

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
					static bool exclude_from_capture = false;
					if (ImGui::Checkbox("exclude from capture", &exclude_from_capture))
					{
						if (exclude_from_capture)
							SetWindowDisplayAffinity(Application::GetWindow().GetHandle(), WDA_EXCLUDEFROMCAPTURE);
						else
							SetWindowDisplayAffinity(Application::GetWindow().GetHandle(), WDA_NONE);
					}
					if (ImGui::Button("internal ui"))
#else
					if (ImGui::Button("external ui"))
#endif
					{
#ifdef TOAD_LOADER
						settings.g_is_ui_internal = true;
						ShowWindow(Application::GetWindow().GetHandle(), SW_HIDE);
#else
						LOGDEBUG("Closing Internal UI and switching to Loader's UI");
						toad::CInternalUI::ShouldClose = true;
#endif
					}
					//keybind_button(&)

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

		if (chest_stealer_slot_info_edit)
		{
			chest_stealer_slotpos_setter(&chest_stealer_slot_info_edit);
		}

		ImGui::End();
	}
}

}