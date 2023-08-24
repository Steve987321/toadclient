#include "config.h"
#include <filesystem>
#include <fstream>

#include "toad.h"

using json = nlohmann::json;

/// Sets the current settings of a json string 
extern void LoadSettings(std::string_view jsonSettings);

/// Returns a json of the current settings
extern json SettingsToJson();

namespace config
{

void LoadConfig(std::string_view path, std::string_view file_name, std::string_view file_ext)
{
	std::ifstream f;
	char fullPath[MAX_PATH] = {};

	memcpy_s(fullPath, MAX_PATH, path.data(), path.length());

	if (!path.ends_with("\\"))
	{
		strncat_s(fullPath, "\\", strlen("\\"));
	}

	strncat_s(fullPath, file_name.data(), file_name.length());
	strncat_s(fullPath, file_ext.data(), file_ext.length());

	f.open(fullPath);

	if (f.is_open())
	{
		std::stringstream ssbuf;
		ssbuf << f.rdbuf();
		f.close();

		LoadSettings(ssbuf.str());
	}
	else
	{
		std::cout << "failed to open file with path: " << fullPath << std::endl;
	}

}

void SaveConfig(std::string_view path, std::string_view file_name, std::string_view file_ext)
{
	std::ofstream f;
	char fullPath[MAX_PATH] = {};

	memcpy_s(fullPath, MAX_PATH, path.data(), path.length());

	if (!path.ends_with("\\"))
	{
		strncat_s(fullPath, "\\", strlen("\\"));
	}

	strncat_s(fullPath, file_name.data(), file_name.length());
	strncat_s(fullPath, file_ext.data(), file_ext.length());

	f.open(fullPath, std::fstream::out | std::fstream::trunc);

	if (f.is_open())
	{
		f << SettingsToJson().dump();
		f.close();
	}
	else if (f.fail())
	{
		std::cout << "Failed to save config to " << fullPath << std::endl;
	}
}

bool LoadConfigFromClipBoard()
{
	if (!OpenClipboard(nullptr))
	{
		std::cout << "Failed opening clipboard\n";
		return false;
	}

	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr)
	{
		std::cout << "Failed getting data [1]\n";
		CloseClipboard();
		return false;
	}

	char* data = static_cast<char*>(GlobalLock(hData));
	if (data == nullptr)
	{
		std::cout << "Failed getting data [2]\n";
		CloseClipboard();
		return false;
	}

	GlobalUnlock(hData);

	CloseClipboard();

	LoadSettings(data);

	return true;

}

void SaveConfigToClipBoard()
{
	std::stringstream ss;
	ss << SettingsToJson().dump();

	int n = ss.str().length() + 1;

	// must be called with GMEM_MOVABLE as stated in the docs
	void* pData = GlobalAlloc(GMEM_MOVEABLE, n);

	if (pData != nullptr)
	{
		if (auto pMemData = GlobalLock(pData); pMemData != nullptr)
		{
			memcpy(pMemData, ss.str().c_str(), n);
			GlobalUnlock(pData);

			if (OpenClipboard(nullptr))
			{
				EmptyClipboard();
				SetClipboardData(CF_TEXT, pData);
				CloseClipboard();
			}
		}
	}

	GlobalFree(pData);
}

std::vector<ConfigFile> GetAllConfigsInDirectory(std::string_view path)
{
	std::vector<ConfigFile> res = {};

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		auto ext = entry.path().extension();
		if (ext == ".toad" || ext == ".txt")
		{
			res.emplace_back(entry.path().stem().string(), entry.last_write_time());
		}
	}

	return res;
}

}

void LoadSettings(std::string_view jsonSettings)
{
	json data;

	try 
	{
		data = json::parse(jsonSettings);
	}
	catch (json::parse_error& e)
	{
		std::cout << "parse error at: " << e.byte << " (" << e.what() << ')' << std::endl;
		return;
	}
	catch (...)
	{
		std::cout << "Unkown error while loading settings\n";
		return;
	}

	using namespace toad;

	// left auto clicker
	left_clicker::enabled = data["lc_enabled"];
	left_clicker::min_cps = data["lc_mincps"];
	left_clicker::max_cps = data["lc_maxcps"];
	left_clicker::break_blocks = data["lc_breakblocks"];
	left_clicker::block_hit = data["lc_blockhit"];
	left_clicker::block_hit_ms = data["lc_blockhitms"];
	left_clicker::block_hit_stop_lclick = data["lc_blockhitpause"];
	left_clicker::targeting_affects_cps = data["lc_smartcps"];
	left_clicker::weapons_only = data["lc_weaponsonly"];
	left_clicker::trade_assist = data["lc_tradeassist"];
	left_clicker::start_break_blocks_reaction = data["lc_bbStart"];
	left_clicker::stop_break_blocks_reaction = data["lc_bbStop"];

	// right auto clicker
	right_clicker::enabled = data["rc_enabled"];
	right_clicker::cps = data["rc_cps"];
	right_clicker::blocks_only = data["rc_blocks_only"];
	right_clicker::start_delayms = data["rc_start_delay"];

	// aim assist
	aa::enabled = data["aa_enabled"];
	aa::distance = data["aa_distance"];
	aa::speed = data["aa_speed"];
	aa::horizontal_only = data["aa_horizontal_only"];
	aa::fov = data["aa_fov"];
	aa::invisibles = data["aa_invisibles"];
	aa::target_mode = data["aa_mode"];
	aa::always_aim = data["aa_always_aim"];
	aa::aim_at_closest_point = data["aa_multipoint"];
	aa::lock_aim = data["aa_lockaim"];
	aa::break_blocks = data["aa_bb"];

	// no click delay
	no_click_delay::enabled = data["ncd_enabled"];

	// bridge assist
	bridge_assist::enabled = data["ba_enabled"];
	bridge_assist::pitch_check = data["ba_pitch_check"];
	bridge_assist::block_check = data["ba_block_check"];

	// blink
	blink::enabled = data["bl_enabled"];
	blink::key = data["bl_key"];
	blink::stop_rec_packets = data["bl_stop_incoming_packets"];
	blink::show_trail = data["bl_show_trail"];
	blink::limit_seconds = data["bl_limit_seconds"];

	// velocity
	velocity::enabled = data["vel_enabled"];
	velocity::jump_reset = data["vel_jumpreset"];
	velocity::horizontal = data["vel_horizontal"];
	velocity::vertical = data["vel_vertical"];
	velocity::chance = data["vel_chance"];
	velocity::delay = data["vel_delay"];

	velocity::enabled = data["vel_enabled"];
	velocity::only_when_clicking = data["vel_onlyclicking"];
	velocity::only_when_moving = data["vel_onlymoving"];
	velocity::kite = data["vel_kite"];
	velocity::jump_reset = data["vel_jumpreset"];
	velocity::jump_press_chance = data["vel_jumpchance"];
	velocity::horizontal = data["vel_horizontal"];
	velocity::vertical = data["vel_vertical"];
	velocity::chance = data["vel_chance"];
	velocity::delay = data["vel_delay"];

	// esp
	esp::enabled = data["esp_enabled"];
	esp::line_col[0] = data["esp_linecolr"];
	esp::line_col[1] = data["esp_linecolg"];
	esp::line_col[2] = data["esp_linecolb"];
	esp::line_col[3] = data["esp_linecola"];
	esp::fill_col[0] = data["esp_fillcolr"];
	esp::fill_col[1] = data["esp_fillcolg"];
	esp::fill_col[2] = data["esp_fillcolb"];
	esp::fill_col[3] = data["esp_fillcola"];
	esp::text_bg_col[0] = data["esp_bgcolr"];
	esp::text_bg_col[1] = data["esp_bgcolg"];
	esp::text_bg_col[2] = data["esp_bgcolb"];
	esp::text_bg_col[3] = data["esp_bgcola"];
	esp::show_name = data["esp_show_name"];
	esp::show_distance = data["esp_show_distance"];
	esp::show_health = data["esp_show_health"];
	esp::esp_mode = data["esp_mode"];
	esp::show_txt_bg = data["esp_bg"];

	// esp extra
	esp::static_esp_width = data["esp_static_width"];
	esp::text_shadow = data["esp_text_shadow"];
	esp::text_col[0] = data["esp_text_colr"];
	esp::text_col[1] = data["esp_text_colg"];
	esp::text_col[2] = data["esp_text_colb"];
	esp::text_col[3] = data["esp_text_cola"];
	esp::text_size = data["esp_fontsize"];
	esp::show_border = data["esp_border"];

	// block esp
	block_esp::enabled = data["blockesp_enabled"];
	json blockArray = data["block_esp_array"];
	std::unordered_map<int, ImVec4> tmpList = {};
	for (const auto& element : blockArray.items())
	{
		int id = std::stoi(element.key());
		float r = element.value().at("x").get<float>();
		float g = element.value().at("y").get<float>();
		float b = element.value().at("z").get<float>();
		float a = element.value().at("w").get<float>();
		tmpList[id] = { r,g,b,a };
	}

	block_esp::block_list = tmpList;
}

json SettingsToJson()
{
	// settings 
	json data;

	// rand
	json lcRandInconsistenties = json::object();
	json lcRandInconsistenties2 = json::object();
	json lcRandBoosts = json::object();

	using namespace toad;

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
		const auto& in = left_clicker::rand.inconsistencies[i];
		lcRandInconsistenties[std::to_string(i)] =
		{
			//float min, float max, int chance, int frequency)
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
			//float min, float max, int chance, int frequency)
			{"nmin", in.min_amount_ms},
			{"nmax", in.max_amount_ms},
			{"c", in.chance},
			{"f", in.frequency}
		};
	}

	data["lc_randb"] = lcRandBoosts;
	data["lc_randi"] = lcRandInconsistenties;
	data["lc_randi2"] = lcRandInconsistenties2;

	// left auto clicker
	data["lc_enabled"] = left_clicker::enabled;
	data["lc_mincps"] = left_clicker::min_cps;
	data["lc_maxcps"] = left_clicker::max_cps;
	data["lc_breakblocks"] = left_clicker::break_blocks;
	data["lc_blockhit"] = left_clicker::block_hit;
	data["lc_blockhitms"] = left_clicker::block_hit_ms;
	data["lc_blockhitpause"] = left_clicker::block_hit_stop_lclick;
	data["lc_smartcps"] = left_clicker::targeting_affects_cps;
	data["lc_weaponsonly"] = left_clicker::weapons_only;
	data["lc_tradeassist"] = left_clicker::trade_assist;
	data["lc_bbStart"] = left_clicker::start_break_blocks_reaction;
	data["lc_bbStop"] = left_clicker::stop_break_blocks_reaction;

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
	data["aa_mode"] = aa::target_mode;
	data["aa_always_aim"] = aa::always_aim;
	data["aa_multipoint"] = aa::aim_at_closest_point;
	data["aa_lockaim"] = aa::lock_aim;
	data["aa_bb"] = aa::break_blocks;

	// no click delay
	data["ncd_enabled"] = no_click_delay::enabled;

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
	data["vel_onlyclicking"] = velocity::only_when_clicking;
	data["vel_onlymoving"] = velocity::only_when_moving;
	data["vel_kite"] = velocity::kite;
	data["vel_jumpreset"] = velocity::jump_reset;
	data["vel_jumpchance"] = velocity::jump_press_chance;
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
	data["esp_show_health"] = esp::show_health;
	data["esp_mode"] = esp::esp_mode;
	data["esp_bg"] = esp::show_txt_bg;
	// esp extra
	data["esp_static_width"] = esp::static_esp_width;
	data["esp_text_shadow"] = esp::text_shadow;
	data["esp_text_colr"] = esp::text_col[0];
	data["esp_text_colg"] = esp::text_col[1];
	data["esp_text_colb"] = esp::text_col[2];
	data["esp_text_cola"] = esp::text_col[3];
	data["esp_bgcolr"] = esp::text_bg_col[0];
	data["esp_bgcolg"] = esp::text_bg_col[1];
	data["esp_bgcolb"] = esp::text_bg_col[2];
	data["esp_bgcola"] = esp::text_bg_col[3];
	data["esp_fontsize"] = esp::text_size;
	data["esp_border"] = esp::show_border;

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

	return data;
}