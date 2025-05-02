#include "pch.h"
#include "ui.h"
#include "Toad/ToadUI.h"
#include "Application/application.h"
#include "Toad/Inject/injector.h"

namespace toad::UI
{
	static std::thread init_thread;

	// for the flickering of the possible minecraft windows
	static std::vector<Window> shown_windows_list = {};

	static void InjectStatusCallback(const std::string& status)
	{
		std::cout << status << std::endl;
	}

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

		ToadUI(io);
	}

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
			Application::ScanWindows();

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
						if (shown_windows_list.size() != g_mc_window_list.size())
							shown_windows_list = g_mc_window_list;

						count = false;
					}
				}
				if (shown_windows_list.size() != g_mc_window_list.size() && !count)
				{
					timer = 0;
					count = true;
				}

				if (shown_windows_list.empty())
				{
					center_text_multi(ImVec4(0.2f, 0.2f, 0.2f, 1), "make sure minecraft is opened. \n Supported clients: Lunar Client (1.8.9 & 1.7.10)");
				}
				else
				{
					for (const auto& window : shown_windows_list)
					{
						auto btn_name = std::string(window.title + " [" + std::to_string(window.pid) + ']');
						if (btn_name.empty()) btn_name = "##";
						if (ImGui::Button(btn_name.c_str()))
						{
							loading = true;

							// get client type
							Injector::inject_status = "getting client type";

							settings.g_curr_client = get_client_type(window.title);
							std::cout << "currclient type: " << (int)settings.g_curr_client << std::endl;
							if (init_thread.joinable())
								init_thread.join();

							init_thread = std::thread([&]
								{
									Injector::inject_status = "init #1";
								
									if (!Application::PreInjectSetup())
										failed_inject = true;
									else if (!Injector::Inject(window.pid, Application::dll_path, InjectStatusCallback))
										failed_inject = true;

									if (!failed_inject)
										g_is_verified = true;

									g_injected_window = window;
									set_injected_window(g_injected_window);
									loading = false;
								});
						}
					}
				}
				if (loading)
				{
					std::string state = "state: " + Injector::inject_status;
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
					show_message_box("failed to inject", Injector::inject_status.c_str(), failed_inject, true, mboxType::ERR);
				}
			}
			ImGui::EndChild(); // mc windows
		}
		ImGui::End(); // main window

		ImGui::PopStyleVar();
	}

}
