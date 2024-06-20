#include "toad.h"

#include "../../ToadClient/vendor/nlohmann/json.hpp"

using namespace toad;

// for ipc to dll 
HANDLE hMapFile = nullptr;
constexpr int bufSize = 3000;

// flag used to call init once
std::once_flag init_once_flag;

// threads for loader 
inline std::thread updateSettingsThread;
inline std::thread windowScannerThread;

// updates settings for dll 
extern void update_settings();

// scan for minecraft instances 
extern void window_scanner();

extern bool is_proc_mc(DWORD dwPID);
extern BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam);

bool toad::pre_init()
{
	std::cout << "Starting window scanner thread\n";
	windowScannerThread = std::thread(window_scanner);
	return true;
}

bool toad::init()
{
	// setup ipc
	static bool once = false;
	if (!once)
	{
		hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bufSize, L"ToadClientMappingObj");
		if (hMapFile == NULL)
		{
			return false;
		}

		LPVOID pMem = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
		if (pMem == NULL)
		{
			CloseHandle(hMapFile);
			return false;
		}

		memset(pMem, L'\0', bufSize);

		UnmapViewOfFile(pMem);

		// update settings for ipc 
		updateSettingsThread = std::thread([]
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

void update_settings()
{
	auto pmem = MapViewOfFile(hMapFile, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
	if (pmem == NULL)
	{
		std::cout << "[!] something went wrong calling MapViewOfFile\n";
		return;
	}

	using json = nlohmann::json;

	auto s = std::string(static_cast<LPCSTR>(pmem));
	json d_in;
	if (!s.empty())
	{
		auto endof = s.find("END");
		std::string settings = s.substr(0, endof);
		d_in = json::parse(settings);
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
		if (n > bufSize)
		{
			std::cout << "not enough space for settings, increase buf size of mapped memory!\n";
			return;
		}
		once = true;
	}
	//OutputDebugStringA(ss.str().c_str());
	memcpy(pmem, ss.str().c_str(), ss.str().length());

	UnmapViewOfFile(pmem);
}

void toad::stop_all_threads()
{
	if (windowScannerThread.joinable()) windowScannerThread.join();
	if (updateSettingsThread.joinable()) updateSettingsThread.join();
}

void toad::clean_up()
{
	CloseHandle(hMapFile);
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

BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam)
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

	if (!toad::g_is_verified) // we haven't injected yet
	{
		if (IsWindowVisible(hwnd) && win_name_length != 0) {
			// convert window title to std::string
			auto title = std::string(windowTitle);

			// title to lower case 
			std::ranges::transform(title, title.begin(), tolower);

			// check if window title contains minecraft client names
			if (title.find("lunar client") != std::string::npos || title.find("minecraft") != std::string::npos)
			{
				// check if important modules exist in the process
				// before we add to the list 
				if (is_proc_mc(PID))
					g_mc_window_list.emplace_back(title, PID, hwnd);
			}

			return TRUE;
		}
	}

	return TRUE;
}

void window_scanner()
{
	while (toad::g_is_running)
	{
		g_mc_window_list.clear();

		static uint8_t checks = 0;
		bool found_injected_window = false;
		EnumWindows(enumWindowCallback, (LPARAM)&found_injected_window);

		if (g_injected_window.pid != 0 && !found_injected_window && checks++ >= 2)
		{
			g_is_verified = false;
			checks = 0;
			g_mc_window_list.clear();
		}

		SLEEP(1000);
	}
}