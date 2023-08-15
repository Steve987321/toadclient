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
	auto pMem = MapViewOfFile(hMapFile, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
	if (pMem == NULL)
	{
		std::cout << "[!] something went wrong calling MapViewOfFile\n";
		return;
	}

	using json = nlohmann::json;

	auto s = std::string(static_cast<LPCSTR>(pMem));
	json datain;
	if (!s.empty())
	{
		auto endof = s.find("END");
		std::string settings = s.substr(0, endof);
		datain = json::parse(settings);
	}

	json dataout;

	if (datain.contains("ui_internal_should_close"))
	{
		if (datain["ui_internal_should_close"])
		{
			if (g_is_ui_internal)
			{
				ShowWindow(AppInstance->GetWindow()->GetHandle(), SW_SHOW);
				dataout["ui_internal_should_close"] = false;
				dataout["ui_internal"] = false;
				g_is_ui_internal = false;
			}
		}
	}

	if (left_clicker::update_rand_flag)
	{
		// rand
		json lcRandInconsistenties = json::object();
		json lcRandInconsistenties2 = json::object();
		json lcRandBoosts = json::object();

		for (int i = 0; i < left_clicker::rand.boosts.size(); i++)
		{
			const auto& b = left_clicker::rand.boosts[i];

			lcRandBoosts[std::to_string(b.id)] =
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
			lcRandInconsistenties[std::to_string(i)] =
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

			lcRandInconsistenties2[std::to_string(i)] =
			{
				{"nmin", in.min_amount_ms},
				{"nmax", in.max_amount_ms},
				{"c", in.chance},
				{"f", in.frequency}
			};
		}

		dataout["lc_randb"] = lcRandBoosts;
		dataout["lc_randi"] = lcRandInconsistenties;
		dataout["lc_randi2"] = lcRandInconsistenties2;
		dataout["updatelcrand"] = true;
	
		if (datain.contains("done"))
			left_clicker::update_rand_flag = false;
	}

	dataout["ui_internal"] = g_is_ui_internal;
	dataout["client_type"] = g_curr_client;

	// left auto clicker
	dataout["lc_enabled"] = left_clicker::enabled;
	dataout["lc_mincps"] = left_clicker::min_cps;
	dataout["lc_maxcps"] = left_clicker::max_cps;
	dataout["lc_breakblocks"] = left_clicker::break_blocks;
	dataout["lc_blockhit"] = left_clicker::block_hit;
	dataout["lc_blockhitms"] = left_clicker::block_hit_ms;
	dataout["lc_smartcps"] = left_clicker::targeting_affects_cps;
	dataout["lc_weaponsonly"] = left_clicker::weapons_only;
	dataout["lc_tradeassist"] = left_clicker::trade_assist;
	
	// right auto clicker
	dataout["rc_enabled"] = right_clicker::enabled;
	dataout["rc_cps"] = right_clicker::cps;
	dataout["rc_blocks_only"] = right_clicker::blocks_only;
	dataout["rc_start_delay"] = right_clicker::start_delayms;

	// aim assist
	dataout["aa_enabled"] = aa::enabled;
	dataout["aa_distance"] = aa::distance;
	dataout["aa_speed"] = aa::speed;
	dataout["aa_horizontal_only"] = aa::horizontal_only;
	dataout["aa_fov"] = aa::fov;
	dataout["aa_invisibles"] = aa::invisibles;
	dataout["aa_mode"] = aa::target_mode;
	dataout["aa_always_aim"] = aa::always_aim;
	dataout["aa_multipoint"] = aa::aim_at_closest_point;
	dataout["aa_lockaim"] = aa::lock_aim;

	// bridge assist
	dataout["ba_enabled"] = bridge_assist::enabled;
	dataout["ba_pitch_check"] = bridge_assist::pitch_check;
	dataout["ba_block_check"] = bridge_assist::block_check;

	// blink
	dataout["bl_enabled"] = blink::enabled;
	dataout["bl_key"] = blink::key;
	dataout["bl_stop_incoming_packets"] = blink::stop_rec_packets;
	dataout["bl_show_trail"] = blink::show_trail;
	dataout["bl_limit_seconds"] = blink::limit_seconds;

	// velocity
	dataout["vel_enabled"] = velocity::enabled;
	dataout["vel_jumpreset"] = velocity::jump_reset;
	dataout["vel_horizontal"] = velocity::horizontal;
	dataout["vel_vertical"] = velocity::vertical;
	dataout["vel_chance"] = velocity::chance;
	dataout["vel_delay"] = velocity::delay;

	// esp
	dataout["esp_enabled"] = esp::enabled;
	dataout["esp_linecolr"] = esp::line_col[0];
	dataout["esp_linecolg"] = esp::line_col[1];
	dataout["esp_linecolb"] = esp::line_col[2];
	dataout["esp_linecola"] = esp::line_col[3];
	dataout["esp_fillcolr"] = esp::fill_col[0];
	dataout["esp_fillcolg"] = esp::fill_col[1];
	dataout["esp_fillcolb"] = esp::fill_col[2];
	dataout["esp_fillcola"] = esp::fill_col[3];
	dataout["esp_show_name"] = esp::show_name;
	dataout["esp_show_distance"] = esp::show_distance;
	dataout["esp_show_health"] = esp::show_health;
	dataout["esp_mode"] = esp::esp_mode;
	// esp extra
	dataout["esp_line_width"] = esp::line_width;
	dataout["esp_static_width"] = esp::static_esp_width;
	dataout["esp_text_shadow"] = esp::text_shadow;
	dataout["esp_text_colr"] = esp::text_col[0];
	dataout["esp_text_colg"] = esp::text_col[1];
	dataout["esp_text_colb"] = esp::text_col[2];
	dataout["esp_text_cola"] = esp::text_col[3];
	// block esp
	dataout["blockesp_enabled"] = block_esp::enabled;
	json blockArray = json::object();
	for (const auto& [id, col] : block_esp::block_list)
	{
		blockArray[std::to_string(id)] =
		{
			{"x", col.x},
			{"y", col.y},
			{"z", col.z},
			{"w", col.w}
		};
	}
	dataout["block_esp_array"] = blockArray;

	std::stringstream ss;
	ss << dataout << "END";

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
	memcpy(pMem, ss.str().c_str(), ss.str().length());

	UnmapViewOfFile(pMem);
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

BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam) {
	constexpr DWORD TITLE_SIZE = 1024;
	DWORD PID = 0;

	CHAR windowTitle[TITLE_SIZE];

	GetWindowTextA(hwnd, windowTitle, TITLE_SIZE);

	const int win_name_length = ::GetWindowTextLength(hwnd);

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
			GetWindowThreadProcessId(hwnd, &PID);
			if (is_proc_mc(PID))
				g_mc_window_list.emplace_back(title, PID, hwnd);
		}

		return TRUE;
	}

	return TRUE;
}

void window_scanner()
{
	while (toad::g_is_running)
	{
		if (!toad::g_is_verified) // we haven't injected yet
		{
			g_mc_window_list.clear();
			EnumWindows(enumWindowCallback, 0);
		}
		SLEEP(1000);
	}
}