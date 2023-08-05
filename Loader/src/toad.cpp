#include "toad.h"

#include "../../ToadClient/nlohmann/json.hpp"

using namespace toad;

// for ipc to dll 
HANDLE hMapFile = nullptr;
constexpr int bufSize = 1000;

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
	json data2;
	if (!s.empty())
	{
		auto endof = s.find("END");
		std::string settings = s.substr(0, endof);
		data2 = json::parse(settings);
	}

	json data;

	if (data2.contains("ui_internal_should_close"))
	{
		if (data2["ui_internal_should_close"])
		{
			if (g_is_ui_internal)
			{
				ShowWindow(AppInstance->GetWindow()->GetHandle(), SW_SHOW);
				data["ui_internal_should_close"] = false;
				data["ui_internal"] = false;
				g_is_ui_internal = false;
			}
		}
	}
	data["ui_internal"] = g_is_ui_internal;
	data["client_type"] = g_curr_client;

	// left auto clicker
	data["lc_enabled"] = left_clicker::enabled;
	data["lc_cps"] = left_clicker::cps;
	data["lc_breakblocks"] = left_clicker::break_blocks;
	data["lc_blockhit"] = left_clicker::block_hit;
	data["lc_blockhitms"] = left_clicker::block_hit_ms;
	data["lc_smartcps"] = left_clicker::targeting_affects_cps;
	data["lc_weaponsonly"] = left_clicker::weapons_only;
	data["lc_tradeassist"] = left_clicker::trade_assist;

	// right auto clicker
	data["rc_enabled"] = right_clicker::enabled;
	data["rc_cps"] = right_clicker::cps;
	data["rc_blocks_only"] = right_clicker::blocks_only;
	data["rc_start_delay"] = right_clicker::start_delayms;

	// aim assist
	data["aa_enabled"] = aa::enabled;
	data["aa_distance"] = aa::distance;
	data["aa_speed"] = aa::speed;
	data["aa_horizontal_only"] = aa::horizontal_only;
	data["aa_fov"] = aa::fov;
	data["aa_invisibles"] = aa::invisibles;
	data["aa_targetFOV"] = aa::targetFOV;
	data["aa_always_aim"] = aa::always_aim;
	data["aa_multipoint"] = aa::aim_at_closest_point;
	data["aa_aim_in_hitbox"] = aa::aim_in_target;
	data["aa_lockaim"] = aa::lock_aim;

	// bridge assist
	data["ba_enabled"] = bridge_assist::enabled;
	data["ba_pitch_check"] = bridge_assist::pitch_check;
	data["ba_block_check"] = bridge_assist::block_check;

	// blink
	data["bl_enabled"] = blink::enabled;
	data["bl_key"] = blink::key;
	data["bl_stop_incoming_packets"] = blink::stop_rec_packets;
	data["bl_show_trail"] = blink::show_trail;
	data["bl_limit_seconds"] = blink::limit_seconds;

	// velocity
	data["vel_enabled"] = velocity::enabled;
	data["vel_jumpreset"] = velocity::jump_reset;
	data["vel_horizontal"] = velocity::horizontal;
	data["vel_vertical"] = velocity::vertical;
	data["vel_chance"] = velocity::chance;
	data["vel_delay"] = velocity::delay;

	// esp
	data["esp_enabled"] = esp::enabled;
	data["esp_linecolr"] = esp::line_col[0];
	data["esp_linecolg"] = esp::line_col[1];
	data["esp_linecolb"] = esp::line_col[2];
	data["esp_linecola"] = esp::line_col[3];
	data["esp_fillcolr"] = esp::fill_col[0];
	data["esp_fillcolg"] = esp::fill_col[1];
	data["esp_fillcolb"] = esp::fill_col[2];
	data["esp_fillcola"] = esp::fill_col[3];
	data["esp_show_name"] = esp::show_name;
	data["esp_show_distance"] = esp::show_distance;
	data["esp_mode"] = esp::esp_mode;
	// esp extra
	data["esp_line_width"] = esp::line_width;
	data["esp_static_width"] = esp::static_esp_width;
	data["esp_text_shadow"] = esp::text_shadow;
	data["esp_text_colr"] = esp::text_col[0];
	data["esp_text_colg"] = esp::text_col[1];
	data["esp_text_colb"] = esp::text_col[2];
	data["esp_text_cola"] = esp::text_col[3];
	// block esp
	data["blockesp_enabled"] = block_esp::enabled;
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
	data["block_esp_array"] = blockArray;

	std::stringstream ss;
	ss << data << "END";
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