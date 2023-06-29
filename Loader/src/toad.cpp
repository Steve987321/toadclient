#include "toad.h"

#include "../../ToadClient/nlohmann/json.hpp"

HANDLE hMapFile = nullptr;
constexpr int bufSize = 1000;

std::shared_mutex mutex;

bool toad::pre_init()
{
	utils::Twin_scan = std::thread(utils::Fwin_scan);
	return true;
}

bool initialized = false;
bool toad::init()
{
	// setup ipc 
	if (initialized) return false;
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bufSize, L"ToadClientMappingObj");
	if (hMapFile == NULL)
		return false;

	LPVOID pMem = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 0);
	if (pMem == NULL)
	{
		CloseHandle(hMapFile);
		return false;
	}
	
	memset(pMem, L'\0', bufSize);

	UnmapViewOfFile(pMem);

	// update settings for ipc 
	Tupdate_settings = std::thread([]
		{
			while (g_is_running)
			{
				Fupdate_settings();
				SLOW_SLEEP(100);
			}
		});

	initialized = true;
	return true;
}

void toad::Fupdate_settings()
{
	std::unique_lock lock(mutex);
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
				ShowWindow(AppInstance->get_window(), SW_SHOW);
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
	data["aa_lockaim"] = aa::lock_aim;

	// auto bridge
	data["ab_enabled"] = auto_bridge::enabled;
	data["ab_pitch_check"] = auto_bridge::pitch_check;

	// blink
	data["bl_enabled"] = blink::enabled;
	data["bl_key"] = blink::key;
	data["bl_stop_on_hit"] = blink::disable_on_hit;
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
	data["esp_linecolr"] = esp::lineCol[0];
	data["esp_linecolg"] = esp::lineCol[1];
	data["esp_linecolb"] = esp::lineCol[2];
	data["esp_linecola"] = esp::lineCol[3];
	data["esp_fillcolr"] = esp::fillCol[0];
	data["esp_fillcolg"] = esp::fillCol[1];
	data["esp_fillcolb"] = esp::fillCol[2];
	data["esp_fillcola"] = esp::fillCol[3];

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
	if (utils::Twin_scan.joinable()) utils::Twin_scan.join();
	if (Tupdate_settings.joinable()) Tupdate_settings.join();
}

void toad::clean_up()
{
	CloseHandle(hMapFile);
}

