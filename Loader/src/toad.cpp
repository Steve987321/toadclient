#include "toad.h"

#include "../../ToadClient/vendor/nlohmann/json.hpp"

namespace toad
{

// for ipc to dll 
static HANDLE hMapFile = nullptr;

// flag used to call init once
static std::once_flag init_once_flag;

// threads for loader 
static std::thread update_settings_thread;
static std::thread check_injected_window;

// updates settings for dll 
extern void update_settings();

extern bool is_proc_mc(DWORD dwPID);
extern BOOL CALLBACK EnumWindowCallback(HWND hwnd, LPARAM lparam);
extern void check_injected_window_open(const Window& window);

bool init()
{
	// setup ipc
	static bool once = false;
	if (!once)
	{
		hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, ipc_bufsize, L"ToadClientMappingObj");
		if (hMapFile == NULL)
		{
			std::cout << "CreateFileMapping returned null: " << GetLastError() << std::endl;
			return false;
		}

		LPVOID pMem = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
		if (pMem == NULL)
		{
			std::cout << "MapViewOfFile returned null: " << GetLastError() << std::endl;
			CloseHandle(hMapFile);
			return false;
		}

		memset(pMem, L'\0', ipc_bufsize);

		UnmapViewOfFile(pMem);

		// update settings for ipc 
		update_settings_thread = std::thread([]
			{
				while (g_is_running)
				{
					update_settings();
					SLEEP(100);
				}
			});

		once = true;
	}
	return true;
}

void scan_windows()
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

void update_settings()
{
	auto pmem = MapViewOfFile(hMapFile, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
	if (pmem == NULL)
	{
		std::cout << "MapViewOfFile returned null: " << GetLastError() << std::endl;
		return;
	}

	using json = nlohmann::json;

	auto s = std::string(static_cast<LPCSTR>(pmem));
	json d_in;
	if (!s.empty())
	{
		auto endof = s.find("END");
		std::string settings = s.substr(0, endof);

		try {
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
			if (g_is_ui_internal)
			{
				ShowWindow(Application::Get()->GetWindow()->GetHandle(), SW_SHOW);
				d_out["ui_internal_should_close"] = false;
				d_out["ui_internal"] = false;
				g_is_ui_internal = false;
			}
		}
	}

	if (left_clicker::update_rand_flag)
	{
		// rand
		json lc_rand_inconsistenties = json::object();
		json lc_rand_inconsistenties2 = json::object();
		json lc_rand_boosts = json::object();

		for (int i = 0; i < left_clicker::rand.boosts.size(); i++)
		{
			const auto& b = left_clicker::rand.boosts[i];

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

		for (int i = 0; i < left_clicker::rand.inconsistencies.size(); i++)
		{
			//float min, float max, int chance, int frequency)

			const auto& in = left_clicker::rand.inconsistencies[i];
			lc_rand_inconsistenties[std::to_string(i)] =
			{
				{"nmin", in.min_amount_ms},
				{"nmax", in.max_amount_ms},
				{"c", in.chance},
				{"f", in.frequency}
			};
		}

		for (int i = 0; i < left_clicker::rand.inconsistencies2.size(); i++)
		{
			const auto& in = left_clicker::rand.inconsistencies2[i];

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
			left_clicker::update_rand_flag = false;
	}

	if (esp::update_font_flag)
	{
		d_out["esp_font"] = esp::font_path.c_str();

		if (d_in.contains("done")) {
			esp::update_font_flag = false;
		}
	}

	// update loader path once 
	static bool once = false;
	if (!once)
	{
		d_out["path"] = loader_path.c_str();
		if (d_in.contains("donepath"))
		{
			once = true;
		}
	}

	d_out["client_type"] = g_curr_client;
	d_out["config"] = loaded_config;

	d_out = config::MergeJson(d_out, config::SettingsToJson());

	std::stringstream ss;
	ss << d_out << "END";

	if (static bool once = false; !once)
	{
		auto n = ss.view().size();
		std::cout << "setting size: " << n << std::endl;
		if (n > ipc_bufsize)
		{
			std::cout << "not enough space for settings, increase buf size of mapped memory!\n";
			ipc_bufsize = n;
			return;
		}
		once = true;
	}
	//OutputDebugStringA(ss.str().c_str());
	memcpy(pmem, ss.str().c_str(), ss.str().length());

	UnmapViewOfFile(pmem);
}

void stop_all_threads()
{
	if (update_settings_thread.joinable()) 
		update_settings_thread.join();

	if (check_injected_window.joinable())
		check_injected_window.join();
}

void clean_up()
{
	CloseHandle(hMapFile);
}

void check_hotkey_press()
{
	if (GetAsyncKeyState(left_clicker::key) & 1)
		left_clicker::enabled = !left_clicker::enabled;
	if (GetAsyncKeyState(right_clicker::key) & 1)
		right_clicker::enabled = !right_clicker::enabled;
	if (GetAsyncKeyState(aa::key) & 1)
		aa::enabled = !aa::enabled;
	if (GetAsyncKeyState(velocity::key) & 1)
		velocity::enabled = !velocity::enabled;
	if (GetAsyncKeyState(bridge_assist::key) & 1)
		bridge_assist::enabled = !bridge_assist::enabled;
	if (GetAsyncKeyState(esp::key) & 1)
		esp::enabled = !esp::enabled;
	if (GetAsyncKeyState(block_esp::key) & 1)
		block_esp::enabled = !block_esp::enabled;
	if (GetAsyncKeyState(blink::key) & 1)
		blink::enabled = !blink::enabled;
	if (GetAsyncKeyState(chest_stealer::key) & 1)
		chest_stealer::enabled = !chest_stealer::enabled;
}

void set_injected_window(const Window& window)
{
	if (check_injected_window.joinable())
		check_injected_window.join();

	check_injected_window = std::thread(check_injected_window_open, std::ref(window));
}

bool is_proc_mc(DWORD dwPID)
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

void check_injected_window_open(const Window& window)
{
	while (g_is_running && FindWindowA(NULL, window.title.c_str()) != NULL)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	int counter = 0;
	while (counter++ < 5)
	{
		if (!g_is_running)
			break;

		if (FindWindowA(NULL, window.title.c_str()) != NULL)
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	g_mc_window_list.clear();
	g_is_verified = false;

}

}
