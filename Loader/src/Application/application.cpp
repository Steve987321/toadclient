#include "pch.h"
#include "application.h"
#include "imgui/imgui.h"

#include "Toad/ToadUI.h"

#include "Toad/Utils/ipc.h"

#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "Toad/Fonts/fa-solid-900Font.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LPDIRECT3D9              g_pD3D;
static LPDIRECT3DDEVICE9        g_pd3dDevice;
static D3DPRESENT_PARAMETERS    g_d3dpp;

namespace toad
{
	extern BOOL CALLBACK EnumWindowCallback(HWND hwnd, LPARAM lparam);

	static std::atomic_bool has_updated_settings = false;
	static std::atomic_bool update_settings_thread_flag = false;
	static Window check_window{"???", 0, NULL};
	static std::thread update_settings_thread;
	static std::thread check_injected_window;
	static std::atomic_bool send_close_flag = false;
	static std::unique_ptr<ImGuiWindow> window = nullptr;

	static void VerifyInjectedWindow(const Window& window)
	{
		while (g_is_running && FindWindowA(NULL, window.title.c_str()) != NULL)
		{
			Sleep(500);
		}

		int counter = 0;
		while (counter++ < 5)
		{
			if (!g_is_running)
				break;

			if (FindWindowA(NULL, window.title.c_str()) != NULL)
				break;

			Sleep(500);
		}

		g_mc_window_list.clear();
		g_is_verified = false;
	}

	static void RenderUI(ImGuiIO* io)
	{
		ImGui::SetCurrentContext(Application::GetWindow().GetImGuiContext());
		g_is_verified ? UI::ui_main(io) : UI::ui_init(io);
	}

	ImGuiWindow& Application::GetWindow()
	{
		return *window;
	}

	bool Application::PreInjectSetup()
	{
		if (!g_ipc.CreateMappedFile("ToadClientMappingObj"))
			return false;

		update_settings_thread_flag = true;
		update_settings_thread = std::thread(Application::UpdateIPC, std::ref(g_ipc));

		while (!has_updated_settings)
			Sleep(1);

		return true;
	}

	void Application::ScanWindows()
	{
		g_mc_window_list.clear();

		static uint8_t checks = 0;
		bool found_injected_window = false;
		EnumWindows(EnumWindowCallback, (LPARAM)&found_injected_window);

		if (g_injected_window.pid != 0 && !found_injected_window && checks++ >= 2)
		{
			g_is_verified = false;
			checks = 0;
			g_mc_window_list.clear();
		}
	}

	void Application::UpdateIPC(Utils::IPC& ipc)
	{
		while (update_settings_thread_flag)
		{
			if (!ipc.OpenMappedFile("ToadClientMappingObj"))
			{
				LOGERROR("OpenMappedFile failed");
				return;
			}

			void* mapped_data = ipc.MapData();

			if (!mapped_data)
				return;

			using json = nlohmann::json;

			std::string s = (char*)mapped_data;
			json d_in;
			if (!s.empty())
			{
				auto endof = s.find("END");
				std::string settings = s.substr(0, endof);

				try 
				{
					d_in = json::parse(settings);
				}
				catch (json::parse_error& e)
				{
					std::cout << "Json parse error: " << e.what() << std::endl;
					return;
				}
			}

			json d_out;

			// don't update these in a loop
			if (d_in.contains("ui_internal_should_close"))
			{
				if (d_in["ui_internal_should_close"])
				{
					if (settings.g_is_ui_internal)
					{
						ShowWindow(Application::GetWindow().GetHandle(), SW_SHOW);
						d_out["ui_internal_should_close"] = false;
						d_out["ui_internal"] = false;
						settings.g_is_ui_internal = false;
					}
				}
			}

			if (settings.lc_update_rand_flag)
			{
				// rand
				json lc_rand_inconsistenties = json::object();
				json lc_rand_inconsistenties2 = json::object();
				json lc_rand_boosts = json::object();

				for (int i = 0; i < settings.lc_rand.boosts.size(); i++)
				{
					const auto& b = settings.lc_rand.boosts[i];

					lc_rand_boosts[std::to_string(b.id)] =
					{
						//float amount, float dur, float transition_dur, Vec2 freq, int id
						{"n", b.amount_ms},
						{"dur", b.duration},
						{"tdur", b.transition_duration},
						{"fqmin", b.freq_min},
						{"fqmax", b.freq_max},
					};
				}

				for (int i = 0; i < settings.lc_rand.inconsistencies.size(); i++)
				{
					//float min, float max, int chance, int frequency)

					const auto& in = settings.lc_rand.inconsistencies[i];
					lc_rand_inconsistenties[std::to_string(i)] =
					{
						{"nmin", in.min_amount_ms},
						{"nmax", in.max_amount_ms},
						{"c", in.chance},
						{"f", in.frequency}
					};
				}

				for (int i = 0; i < settings.lc_rand.inconsistencies2.size(); i++)
				{
					const auto& in = settings.lc_rand.inconsistencies2[i];

					lc_rand_inconsistenties2[std::to_string(i)] =
					{
						{"nmin", in.min_amount_ms},
						{"nmax", in.max_amount_ms},
						{"c", in.chance},
						{"f", in.frequency}
					};
				}

				d_out["lc_randb"] = lc_rand_boosts;
				d_out["lc_randi"] = lc_rand_inconsistenties;
				d_out["lc_randi2"] = lc_rand_inconsistenties2;
				d_out["updatelcrand"] = 1;

				if (d_in.contains("done"))
					settings.lc_update_rand_flag = false;
			}

			if (settings.esp_update_font_flag)
			{
				d_out["esp_font"] = settings.esp_font_path.c_str();

				if (d_in.contains("done")) {
					settings.esp_update_font_flag = false;
				}
			}

			// update loader path once 
			static bool once = false;
			if (!once)
			{
				d_out["path"] = settings.loader_path.c_str();
				if (d_in.contains("donepath"))
				{
					once = true;
				}
			}

			d_out["client_type"] = settings.g_curr_client;
			d_out["config"] = settings.loaded_config;

			if (!g_is_running)
			{
				d_out["close"] = true;
				send_close_flag = true;
			}

			d_out = settings.MergeJson(d_out, settings.SettingsToJson());

			std::stringstream ss;
			ss << d_out << "END";

			if (static bool once = false; !once)
			{
				auto n = ss.view().size();
				std::cout << "setting size: " << n << std::endl;
				if (n > settings.ipc_bufsize)
				{
					std::cout << "not enough space for settings, increase buf size of mapped memory!\n";
					settings.ipc_bufsize = n;
					return;
				}
				once = true;
			}

			memcpy(mapped_data, ss.str().c_str(), ss.str().length());

			ipc.UnMapData();

			has_updated_settings = true;
			Sleep(100);
		}
	}

	bool Application::Init()
    {
        using json = nlohmann::json;
		
		window = std::make_unique<ImGuiWindow>("Toad", Application::WINDOW_HEIGHT, Application::WINDOW_WIDTH);

		// Auto load configs
		settings.loader_path = std::filesystem::current_path().string();
		auto configs = Config::GetAllConfigsInDirectory(settings.loader_path);
		if (!configs.empty())
		{
			// load the only available config 
			if (configs.size() == 1)
			{
				settings.LoadConfig(settings.loader_path, configs.begin()->FileNameStem);
			}
			else
			{
                // Load last edited config
				auto it = std::ranges::min_element(configs, [](const ConfigFile& a, const ConfigFile& b) {return a.LastWrite > b.LastWrite; });

				if (it != configs.end())
				{
					settings.LoadConfig(settings.loader_path, it->FileNameStem);
				}
			}
		}

        // Get Toad DLL path
        dll_path = get_exe_path().parent_path() / "ToadClient.dll";
        if (!std::filesystem::exists(dll_path))
            return false;

        g_is_running = true;

        window->SetUI(RenderUI);
        window->StartWindow();

        return true;
    }

    void Application::MainLoop()
    {
        while (window->IsActive())
        {
            check_hotkey_press();
			SLEEP(1);
        }
    }

    void Application::Exit()
    {
		static std::shared_mutex mutex;
        std::unique_lock lock(mutex);

        if (!g_is_running)
        {
            return;
        }

        std::cout << "closing\n";
        g_is_running = false;

		g_ipc.UnMapData();
		g_ipc.CloseFileMap();

        window->DestroyWindow();

        stop_all_threads();
    }

	void stop_all_threads()
	{
		if (update_settings_thread_flag)
			while (!send_close_flag)
				Sleep(1);

		update_settings_thread_flag = false;
		if (update_settings_thread.joinable())
			update_settings_thread.join();

		if (check_injected_window.joinable())
			check_injected_window.join();
	}

	void check_hotkey_press()
	{
		if (GetAsyncKeyState(settings.lc_key) & 1)
			settings.lc_enabled = !settings.lc_enabled;
		if (GetAsyncKeyState(settings.rc_key) & 1)
			settings.rc_enabled = !settings.rc_enabled;
		if (GetAsyncKeyState(settings.aa_key) & 1)
			settings.aa_enabled = !settings.aa_enabled;
		if (GetAsyncKeyState(settings.vel_key) & 1)
			settings.vel_enabled = !settings.vel_enabled;
		if (GetAsyncKeyState(settings.ba_key) & 1)
			settings.ba_enabled = !settings.ba_enabled;
		if (GetAsyncKeyState(settings.esp_key) & 1)
			settings.esp_enabled = !settings.esp_enabled;
		if (GetAsyncKeyState(settings.besp_key) & 1)
			settings.besp_enabled = !settings.besp_enabled;
		if (GetAsyncKeyState(settings.bl_key) & 1)
			settings.bl_enabled = !settings.bl_enabled;
		if (GetAsyncKeyState(settings.cs_key) & 1)
			settings.cs_enabled = !settings.cs_enabled;
	}

	static const Window* injected_window;
	const Window* get_injected_window()
	{
		return injected_window;
	}

	void set_injected_window(const Window& window)
	{
		injected_window = &window;
		if (check_injected_window.joinable())
			check_injected_window.join();

		check_injected_window = std::thread(VerifyInjectedWindow, std::ref(window));
	}
	
	static bool is_proc_mc(DWORD dwPID)
	{
		auto hModuleSnap = INVALID_HANDLE_VALUE;
		MODULEENTRY32 me32;

		// Take a snapshot of all modules in the specified process.
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		// Set the size of the structure before using it.
		me32.dwSize = sizeof(MODULEENTRY32);

		// Retrieve information about the first module,
		// and exit if unsuccessful
		if (!Module32First(hModuleSnap, &me32))
		{
			std::cout << "Module32First was unsucessful \n";
			CloseHandle(hModuleSnap);
			return false;
		}

		// Check the module list of the process for jvm.dll or javaw.exe 
		do
		{
			if (wcscmp(me32.szModule, L"jvm.dll") == 0 || wcscmp(me32.szModule, L"javaw.exe") == 0)
				return true;
		} while (Module32Next(hModuleSnap, &me32));

		// We haven't found jvm.dll or javaw.exe in the process module list 
		CloseHandle(hModuleSnap);
		return false;
	}

	BOOL CALLBACK EnumWindowCallback(HWND hwnd, LPARAM lparam)
	{
		DWORD PID = 0;
		GetWindowThreadProcessId(hwnd, &PID);

		// check if minecraft was closed 
		if (g_injected_window.pid != 0)
		{
			if (g_injected_window.pid == PID)
			{
				*(bool*)lparam = true;
				return TRUE;
			}
		}

		const static DWORD TITLE_SIZE = 1024;
		CHAR windowTitle[TITLE_SIZE];
		GetWindowTextA(hwnd, windowTitle, TITLE_SIZE);
		const int win_name_length = GetWindowTextLength(hwnd);

		if (!g_is_verified) // we haven't injected yet
		{
			if (IsWindowVisible(hwnd) && win_name_length != 0) {
				// convert window title to std::string
				auto title = std::string(windowTitle);

				// title to lower case 
				std::ranges::transform(title, title.begin(), tolower);

				// check if window title contains minecraft client names
				//if (title.find("lunar client") != std::string::npos || title.find("minecraft") != std::string::npos || title.find("1.8.9") != std::string::npos || title.find("1.7.10")
				//{
					// check if important modules exist in the process
					// before we add to the list 
				if (is_proc_mc(PID))
					g_mc_window_list.emplace_back(title, PID, hwnd);
				//}

				return TRUE;
			}
		}

		return TRUE;
	}
}
