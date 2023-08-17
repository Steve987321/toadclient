#ifdef TOAD_LOADER
#include "toad.h"
#include "Application.h"
#include "inject/injector.h"
#endif

std::thread init_thread;

namespace toad::ui
{

#ifdef TOAD_LOADER
    // ui when injected
    void ui_main(const ImGuiIO* io)
    {
        if (static bool once = false; !once)
        {
            if (init_thread.joinable()) init_thread.join();
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

                            g_curr_client = GetClientType(window.title);
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
                    show_message_box("Injecting", state.c_str(), loading, false);
                    ImGui::SetCursorPos({ get_middle_point().x - 10, get_middle_point().y - 10 });
                    load_spinner("##injectingSpinner", 10, 3, IM_COL32(100, 100, 100, 255));
                }
                else if (failed_shared_mem)
                {
                    if (init_thread.joinable()) init_thread.join();
                    loading = false;
                    show_message_box("failed to initialize", "failed setting up ipc", failed_shared_mem, true, mboxType::ERR);
                }
                else if (failed_inject)
                {
                    if (init_thread.joinable()) init_thread.join();
                    loading = false;
                    show_message_box("failed to inject", inject_status.c_str(), failed_inject, true, mboxType::ERR);
                }
                else if (invalid_client_type)
                {
                    if (init_thread.joinable()) init_thread.join();
                    loading = false;
                    show_message_box("failed", "client is not supported", invalid_client_type, true, mboxType::ERR);
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
            const int incN = left_clicker::rand.inconsistencies.size();
            const int inc2N = left_clicker::rand.inconsistencies2.size();
            const int boostsN = left_clicker::rand.boosts.size();

            static std::queue<int> removeInconsistencyQueue = {};
            static std::queue<int> removeInconsistency2Queue = {};
            static std::queue<int> removeBoostQueue = {};

            if (ImGui::TreeNode(("Inconsistencies (mouse down) " + std::to_string(incN)).c_str()))
            {
                for (int i = 0; i < incN; i++)
                {
                    ImGui::PushID(i);
                    if (ImGui::Button("X"))
                    {
                        removeInconsistencyQueue.push(i);
                    }
                    ImGui::PopID();

                    ImGui::SameLine();

                    auto& in = left_clicker::rand.inconsistencies[i];

                    in.min_amount_ms = std::clamp(in.min_amount_ms, -left_clicker::rand.min_delay + 1.0f, in.max_amount_ms);
                    in.max_amount_ms = std::clamp(in.max_amount_ms, in.min_amount_ms, 500.f);

                    if (ImGui::TreeNode(("inconsistency" + std::to_string(i)).c_str(), "inconsistency (%.1f, %.1f)", in.min_amount_ms, in.max_amount_ms))
                    {
                        ImGui::PushID(i);

                        ImGui::SliderFloat("min delay", &in.min_amount_ms, -left_clicker::rand.min_delay + 1.0f, 500.f, "%.3fms");
                        ImGui::SliderFloat("max delay", &in.max_amount_ms, -left_clicker::rand.min_delay + 1.0f, 500.f, "%.3fms");

                        ImGui::SliderInt("chance", &in.chance, 0, 100, "%d%%");
                        ImGui::SliderInt("frequency", &in.frequency, 1, 150, "every %d clicks");

                        ImGui::PopID();

                        ImGui::TreePop();
                    }
                }

                while (!removeInconsistencyQueue.empty())
                {
                    const auto index = removeInconsistencyQueue.front();
                    left_clicker::rand.inconsistencies.erase(left_clicker::rand.inconsistencies.begin() + index);
                    removeInconsistencyQueue.pop();
                }

                if (ImGui::Button("+"))
                {
                    left_clicker::rand.inconsistencies.emplace_back(0, 0, 50, 20);
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode(("Inconsistencies (mouse up) " + std::to_string(inc2N)).c_str()))
            {
                for (int i = 0; i < inc2N; i++)
                {
                    ImGui::PushID(i);
                    if (ImGui::Button("X"))
                    {
                        removeInconsistency2Queue.push(i);
                    }
                    ImGui::PopID();

                    ImGui::SameLine();

                    auto& in = left_clicker::rand.inconsistencies2[i];

                    in.min_amount_ms = std::clamp(in.min_amount_ms, -left_clicker::rand.min_delay + 1.0f, in.max_amount_ms);
                    in.max_amount_ms = std::clamp(in.max_amount_ms, in.min_amount_ms, 500.f);

                    if (ImGui::TreeNode(("inconsistency" + std::to_string(i)).c_str(), "inconsistency (%.1f, %.1f)", in.min_amount_ms, in.max_amount_ms))
                    {
                        ImGui::PushID(i);

                        ImGui::SliderFloat("min delay", &in.min_amount_ms, -left_clicker::rand.min_delay + 1.0f, 500.f, "%.3fms");
                        ImGui::SliderFloat("max delay", &in.max_amount_ms, -left_clicker::rand.min_delay + 1.0f, 500.f, "%.3fms");

                        ImGui::SliderInt("chance", &in.chance, 0, 100, "%d%%");
                        ImGui::SliderInt("frequency", &in.frequency, 1, 150, "every %d clicks");

                        ImGui::PopID();

                        ImGui::TreePop();
                    }
                }

                while (!removeInconsistency2Queue.empty())
                {
                    const auto index = removeInconsistency2Queue.front();
                    left_clicker::rand.inconsistencies2.erase(left_clicker::rand.inconsistencies2.begin() + index);
                    removeInconsistency2Queue.pop();
                }

                if (ImGui::Button("+"))
                {
                    left_clicker::rand.inconsistencies2.emplace_back(0, 0, 50, 20);
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode(("Boosts/Drops " + std::to_string(boostsN)).c_str()))
            {
                for (int i = 0; i < boostsN; i++)
                {
                    ImGui::PushID(i);
                    if (ImGui::Button("X"))
                    {
                        removeBoostQueue.push(i);
                    }
                    ImGui::PopID();

                    ImGui::SameLine();

                    auto& b = left_clicker::rand.boosts[i];

                    b.freq_min = std::clamp(b.freq_min, 5, b.freq_max);
                    b.freq_max = std::clamp(b.freq_max, b.freq_min, 200);

                    if (ImGui::TreeNode(("boost" + std::to_string(b.id)).c_str(), "boost (%.1f)", b.amount_ms))
                    {
                        ImGui::PushID(i);

                        ImGui::SliderFloat("amount", &b.amount_ms, 0.1f, 500.f, "%.3f");
                        ImGui::SliderInt("duration", &b.duration, 5, 100, "%d clicks");
                        ImGui::SliderInt("transition duration", &b.transition_duration, 5, 100, "%d clicks");
                        ImGui::SliderInt("freq min", &b.freq_min, 5, 200, "%d%%");
                        ImGui::SliderInt("freq max", &b.freq_max, 5, 200, "%d%%");

                        ImGui::PopID();

                        ImGui::TreePop();
                    }
                }

                while (!removeBoostQueue.empty())
                {
                    const auto index = removeBoostQueue.front();
                    left_clicker::rand.boosts.erase(left_clicker::rand.boosts.begin() + index);
                    removeBoostQueue.pop();
                }

                if (ImGui::Button("+"))
                {
                    left_clicker::rand.boosts.emplace_back(0.5f, 5, 50, 20, 50, boostsN);
                }
                ImGui::TreePop();
            }
            if (ImGui::Button("Update Rand"))
            {
                vClick.SetRand(left_clicker::rand);
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
                showRand = false,
                showGraph = false,
                showPlayback = false;

            vClick.dTime = ImGui::GetIO().DeltaTime;
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Extra"))
                {
                    ImGui::MenuItem("Show Graph", nullptr, &showGraph);
                    ImGui::MenuItem("Show Playback", nullptr, &showPlayback);
                    ImGui::MenuItem("Show Rand Status", nullptr, &showRand);

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            static bool isPlaying = false;
            if (ImGui::Button(isPlaying ? "pause" : "play"))
            {
                isPlaying = !isPlaying;
                isPlaying ? vClick.Start() : vClick.Stop();
            }

            if (isPlaying)
            {

            }

            ImGui::Text("CPS: %d", vClick.GetCPS());

            const auto& rand = vClick.GetRand();
            ImGui::Text("range(%f - %f) | delay: %f", rand.edited_min, rand.edited_max, rand.delay);
            ImGui::Text("inconsistency delay: %f", rand.inconsistency_delay);

            ImGui::End();
        }
    }

    ImGui::FileBrowser espFontDialog;

    void esp_visualizer(bool* enabled)
    {
        static bool once = false;
        if (!once)
        {
            espFontDialog.SetTitle("select a TrueType Font");
            espFontDialog.SetTypeFilters({ ".ttf" });
            once = true;
        }

        ImGui::Begin("ESP box settings", enabled, ImGuiWindowFlags_NoSavedSettings);

        const auto window_size = ImGui::GetWindowSize();
        const float window_spacing = 20;

        // for visualization
        static int visualize_health = 20;

        ImGui::BeginChild("esp box visual", { window_size.x / 2 - window_spacing, window_size.y - window_spacing - 20 }, true);
        {
            const auto window_pos = ImGui::GetWindowPos();

            const auto childsize = ImGui::GetWindowSize();
            // box
            ImVec2 min = { window_pos.x + window_spacing + 20 , window_pos.y + window_spacing + 20 };
            ImVec2 max = { window_pos.x + childsize.x - window_spacing - 20,  window_pos.y + childsize.y - window_spacing - 20 };
            ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::GetColorU32({ esp::fill_col[0], esp::fill_col[1], esp::fill_col[2], esp::fill_col[3] }));

            if (esp::show_name)
            {
                const auto text_col_imu32 = ImGui::GetColorU32({ esp::text_col[0], esp::text_col[1], esp::text_col[2], esp::text_col[3] });
                const auto halfsizex = ImGui::CalcTextSize("Player").x / 2;

                if (esp::text_shadow)
                {
                    ImGui::GetWindowDrawList()->AddText({ (min.x + max.x) / 2 - halfsizex - 1, min.y -1 }, text_col_imu32, "Player");
                    ImGui::GetWindowDrawList()->AddText({ (min.x + max.x) / 2 - halfsizex + 1, min.y +1 }, text_col_imu32, "Player");
                }
                ImGui::GetWindowDrawList()->AddText({ (min.x + max.x) / 2 - halfsizex, min.y }, text_col_imu32, "Player");
            }
            if (esp::show_distance)
            {
                const auto text_col_imu32 = ImGui::GetColorU32({ esp::text_col[0], esp::text_col[1], esp::text_col[2], esp::text_col[3] });

                const auto halfsizex = ImGui::CalcTextSize("5.0").x / 2;
                ImGui::GetWindowDrawList()->AddText({ (min.x + max.x) / 2 - halfsizex, min.y - 10 }, text_col_imu32, "5.0");
            }
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

                // top: green -> middle: yellow -> bottom: red
                ImGui::GetWindowDrawList()->AddRectFilled({ max.x + 2, std::lerp(max.y, min.y, t) }, { max.x + 4, max.y }, ImGui::GetColorU32(col));
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

                ImGui::SliderInt("Health", &visualize_health, 0, 20, "%d HP");

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Box & Text Properties"))
            {
                ImGui::ColorEdit4("text color", esp::text_col, color_edit_flags);
                ImGui::ColorEdit4("text bg color", esp::text_bg_col, color_edit_flags);
                ImGui::Checkbox("text shadow", &esp::text_shadow);
                ImGui::Checkbox("text bg", &esp::text_bg);
                ImGui::SliderFloat("text size", &esp::text_size, 10, 30, "%.1f");

                ImGui::SliderFloat("static 2d box width", &esp::static_esp_width, -10, 10);
                ImGui::SliderInt("line box width", &esp::line_width, 1, 10);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("font"))
            {
                if (ImGui::Button("..."))
                {
                    espFontDialog.Open();
                }

                if (espFontDialog.HasSelected())
                {
					esp::font_path = espFontDialog.GetSelected().string();
                    espFontDialog.ClearSelected();
                    esp::update_font_flag = true;
                }

                if (esp::update_font_flag)
                {
                    load_spinner("update font spinner", 10, 2, IM_COL32_WHITE);
                }

                ImGui::TreePop();
            }

        }
        ImGui::EndChild();

        ImGui::End();

        espFontDialog.Display();
    }
}
