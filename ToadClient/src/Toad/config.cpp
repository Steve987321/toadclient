#include "pch.h"
#include "config.h"

namespace toad
{

using json = nlohmann::json;

template<typename T>
struct ConfigVar
{
	std::string_view name;
	T& v;
};

Config& Config::Get()
{
	static Config instance;
	return instance;
}

void Config::RegisterConfigVars()
{

}

bool Config::LoadSettings(std::string_view jsonSettings, std::string& error_msg)
{
	json data;

	try
	{
		data = json::parse(jsonSettings);
	}
	catch (json::parse_error& e)
	{
		error_msg = "parse error at: " + std::to_string(e.byte) + " (" + e.what() + ')';
		return false;
	}
	catch (...)
	{
		error_msg = "Unkown error while loading settings";
		return false;
	}

	using namespace toad;

	g_is_ui_internal = data.at("ui_internal");

	// left auto clicker
	GetJsonElement(lc_enabled, data, "lc_enabled");
	GetJsonElement(lc_min_cps, data, "lc_mincps");
	GetJsonElement(lc_max_cps, data, "lc_maxcps");
	GetJsonElement(lc_break_blocks, data, "lc_breakblocks");
	GetJsonElement(lc_block_hit, data, "lc_blockhit");
	GetJsonElement(lc_block_hit_ms, data, "lc_blockhitms");
	GetJsonElement(lc_block_hit_stop_lclick, data, "lc_blockhitpause");
	GetJsonElement(lc_targeting_affects_cps, data, "lc_smartcps");
	GetJsonElement(lc_click_check, data, "lc_check");
	GetJsonElement(lc_weapons_only, data, "lc_weaponsonly");
	GetJsonElement(lc_trade_assist, data, "lc_tradeassist");
	GetJsonElement(lc_start_break_blocks_reaction, data, "lc_bbStart");
	GetJsonElement(lc_stop_break_blocks_reaction, data, "lc_bbStop");

	// right auto clicker
	GetJsonElement(rc_enabled, data, "rc_enabled");
	GetJsonElement(rc_cps, data, "rc_cps");
	GetJsonElement(rc_blocks_only, data, "rc_blocksonly");
	GetJsonElement(rc_start_delayms, data, "rc_startdelay");
	GetJsonElement(rc_click_check, data, "rc_check");

	// rand 
	json lcRandBoosts;
	json lcRandInconsistencies;
	json lcRandInconsistencies2;

	//json lcRandBoosts = data.at("lc_randb");
	GetJsonElement(lcRandBoosts, data, "lc_randb");
	GetJsonElement(lcRandInconsistencies, data, "lc_randi");
	GetJsonElement(lcRandInconsistencies2, data, "lc_randi2");
	//json lcRandInconsistencies = data.at("lc_randi");
	//json lcRandInconsistencies2 = data.at("lc_randi2");

	lc_rand.boosts.clear();
	lc_rand.inconsistencies.clear();
	lc_rand.inconsistencies2.clear();

	for (auto& item : lcRandBoosts.items())
	{
		int id = std::stoi(item.key());
		float amount_ms = item.value().at("n");
		float duration = item.value().at("dur");
		float transition_duration = item.value().at("tdur");
		float freq_min = item.value().at("fqmin");
		float freq_max = item.value().at("fqmax");
		lc_rand.boosts.emplace_back(amount_ms, duration, transition_duration, freq_min, freq_max, id);
	}

	for (auto& item : lcRandInconsistencies.items())
	{
		float min_amount_ms = item.value().at("nmin");
		float max_amount_ms = item.value().at("nmax");
		float chance = item.value().at("c");
		float frequency = item.value().at("f");
		lc_rand.inconsistencies.emplace_back(min_amount_ms, max_amount_ms, chance, frequency);
	}

	for (auto& item : lcRandInconsistencies2.items())
	{
		float min_amount_ms = item.value().at("nmin");
		float max_amount_ms = item.value().at("nmax");
		float chance = item.value().at("c");
		float frequency = item.value().at("f");
		lc_rand.inconsistencies2.emplace_back(min_amount_ms, max_amount_ms, chance, frequency);
	}

	lc_update_rand_flag = true;

	// aim assist
	GetJsonElement(aa_enabled, data, "aa_enabled");
	GetJsonElement(aa_distance, data, "aa_distance");
	GetJsonElement(aa_speed, data, "aa_speed");
	GetJsonElement(aa_horizontal_only, data, "aa_horizontal_only");
	GetJsonElement(aa_fov, data, "aa_fov");
	GetJsonElement(aa_invisibles, data, "aa_invisibles");
	GetJsonElement(aa_target_mode, data, "aa_mode");
	GetJsonElement(aa_always_aim, data, "aa_always_aim");
	GetJsonElement(aa_aim_at_closest_point, data, "aa_multipoint");
	GetJsonElement(aa_lock_aim, data, "aa_lockaim");
	GetJsonElement(aa_break_blocks, data, "aa_bb");

	// no click delay
	GetJsonElement(ncd_enabled, data, "ncd_enabled");

	// bridge assist
	GetJsonElement(ba_enabled, data, "ba_enabled");
	GetJsonElement(ba_pitch_check, data, "ba_pitch_check");
	GetJsonElement(ba_block_check, data, "ba_block_check");
	GetJsonElement(ba_only_initiate_when_sneaking, data, "ba_sneak");

	// chest stealer
	GetJsonElement(cs_enabled, data, "cs_enabled");
	GetJsonElement(cs_average_slowness_ms, data, "cs_delay");
	GetJsonElement(cs_items_to_grab, data, "cs_items");
	GetJsonElement(cs_steal_key, data, "cs_key");
	GetJsonElement(cs_show_slot_positions, data, "cs_show_slot_pos");

	json slot_info;
	GetJsonElement(slot_info, data, "cs_slot_info");
	if (slot_info.size() != cs_slot_info.size())
	{
		std::cout << "Resize slot info to " << slot_info.size() << std::endl;
		cs_slot_info.resize(slot_info.size());
	}

	int index = 0;
	for (const auto& item : slot_info) {
		ChestStealerSlotLocationInfo info;
		info.begin_x = item.at("beginx");
		info.begin_y = item.at("beginy");
		info.space_x = item.at("spacex");
		info.space_y = item.at("spacey");
		info.res_x = item.at("resx");
		info.res_y = item.at("resy");

		cs_slot_info[index++] = info;
	}

	// blink
	GetJsonElement(bl_enabled, data, "bl_enabled");
	GetJsonElement(bl_hold_key, data, "bl_key");
	GetJsonElement(bl_stop_rec_packets, data, "bl_stop_incoming_packets");
	GetJsonElement(bl_show_trail, data, "bl_show_trail");
	GetJsonElement(bl_limit_seconds, data, "bl_limit_seconds");

	// velocity
	GetJsonElement(vel_enabled, data, "vel_enabled");
	GetJsonElement(vel_jump_reset, data, "vel_jumpreset");
	GetJsonElement(vel_horizontal, data, "vel_horizontal");
	GetJsonElement(vel_vertical, data, "vel_vertical");
	GetJsonElement(vel_chance, data, "vel_chance");
	GetJsonElement(vel_delay, data, "vel_delay");

	GetJsonElement(vel_enabled, data, "vel_enabled");
	GetJsonElement(vel_only_when_clicking, data, "vel_onlyclicking");
	GetJsonElement(vel_only_when_moving, data, "vel_onlymoving");
	GetJsonElement(vel_kite, data, "vel_kite");
	GetJsonElement(vel_jump_reset, data, "vel_jumpreset");
	GetJsonElement(vel_jump_press_chance, data, "vel_jumpchance");
	GetJsonElement(vel_horizontal, data, "vel_horizontal");
	GetJsonElement(vel_vertical, data, "vel_vertical");
	GetJsonElement(vel_chance, data, "vel_chance");
	GetJsonElement(vel_delay, data, "vel_delay");

	// ui
	GetJsonElement(ui_show_array_list, data, "ui_list");
	GetJsonElement(ui_show_water_mark, data, "ui_mark");

	// esp
	GetJsonElement(esp_enabled, data, "esp_enabled");
	GetJsonElement(esp_line_col[0], data, "esp_linecolr");
	GetJsonElement(esp_line_col[1], data, "esp_linecolg");
	GetJsonElement(esp_line_col[2], data, "esp_linecolb");
	GetJsonElement(esp_line_col[3], data, "esp_linecola");
	GetJsonElement(esp_fill_col[0], data, "esp_fillcolr");
	GetJsonElement(esp_fill_col[1], data, "esp_fillcolg");
	GetJsonElement(esp_fill_col[2], data, "esp_fillcolb");
	GetJsonElement(esp_fill_col[3], data, "esp_fillcola");
	GetJsonElement(esp_text_bg_col[0], data, "esp_bgcolr");
	GetJsonElement(esp_text_bg_col[1], data, "esp_bgcolg");
	GetJsonElement(esp_text_bg_col[2], data, "esp_bgcolb");
	GetJsonElement(esp_text_bg_col[3], data, "esp_bgcola");
	GetJsonElement(esp_show_name, data, "esp_show_name");
	GetJsonElement(esp_show_distance, data, "esp_show_distance");
	GetJsonElement(esp_show_health, data, "esp_show_health");
	GetJsonElement(esp_show_sneaking, data, "esp_show_sneak");
	GetJsonElement(esp_esp_mode, data, "esp_mode");
	GetJsonElement(esp_show_txt_bg, data, "esp_bg");

	// esp extra
	GetJsonElement(esp_static_esp_width, data, "esp_static_width");
	GetJsonElement(esp_text_shadow, data, "esp_text_shadow");
	GetJsonElement(esp_text_col[0], data, "esp_text_colr");
	GetJsonElement(esp_text_col[1], data, "esp_text_colg");
	GetJsonElement(esp_text_col[2], data, "esp_text_colb");
	GetJsonElement(esp_text_col[3], data, "esp_text_cola");
	GetJsonElement(esp_text_size, data, "esp_fontsize");
	GetJsonElement(esp_show_border, data, "esp_border");

	// block esp
	json block_array;

	GetJsonElement(besp_enabled, data, "blockesp_enabled");
	if (GetJsonElement(block_array, data, "block_esp_array"))
	{
		std::unordered_map<int, ImVec4> tmpList = {};
		for (const auto& element : block_array.items())
		{
			int id = 0;
			id = std::stoi(element.key());
			float r = element.value().at("x");
			float g = element.value().at("y");
			float b = element.value().at("z");
			float a = element.value().at("w");
			tmpList[id] = { r,g,b,a };
		}

		besp_block_list = tmpList;
	}

	if (GetJsonElement(lc_key, data, "lc_key"))
	{
		GetJsonElement(rc_key, data, "rc_key");
		GetJsonElement(aa_key, data, "aa_key");
		GetJsonElement(ncd_key, data, "ncd_key");
		GetJsonElement(ba_key, data, "ba_key");
		GetJsonElement(cs_key, data, "cs_key");
		GetJsonElement(bl_key, data, "bl_key");
		GetJsonElement(vel_key, data, "vel_key");
		GetJsonElement(ui_show_array_list_key, data, "ui_key");
		GetJsonElement(esp_key, data, "esp_key");
		return true;
	}

	return true;
}

json Config::MergeJson(const json& a, const json& b)
{
	json result = a.flatten();
	json tmp = b.flatten();

	for (json::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		result[it.key()] = it.value();
	}

	return result.unflatten();
}

json Config::SettingsToJson(bool include_keybinds)
{
	// settings 
	json data;

	// rand
	json lcRandInconsistencies = json::object();
	json lcRandInconsistencies2 = json::object();
	json lcRandBoosts = json::object();

	using namespace toad;

	for (int i = 0; i < lc_rand.boosts.size(); i++)
	{
		const auto& b = lc_rand.boosts[i];

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

	for (int i = 0; i < lc_rand.inconsistencies.size(); i++)
	{
		const auto& in = lc_rand.inconsistencies[i];
		lcRandInconsistencies[std::to_string(i)] =
		{
			//float min, float max, int chance, int frequency)
			{"nmin", in.min_amount_ms},
			{"nmax", in.max_amount_ms},
			{"c", in.chance},
			{"f", in.frequency}
		};
	}

	for (int i = 0; i < lc_rand.inconsistencies2.size(); i++)
	{
		const auto& in = lc_rand.inconsistencies2[i];

		lcRandInconsistencies2[std::to_string(i)] =
		{
			//float min, float max, int chance, int frequency)
			{"nmin", in.min_amount_ms},
			{"nmax", in.max_amount_ms},
			{"c", in.chance},
			{"f", in.frequency}
		};
	}

	data["ui_internal"] = g_is_ui_internal;

	data["lc_randb"] = lcRandBoosts;
	data["lc_randi"] = lcRandInconsistencies;
	data["lc_randi2"] = lcRandInconsistencies2;

	// left auto clicker
	data["lc_enabled"] = lc_enabled;
	data["lc_mincps"] = lc_min_cps;
	data["lc_maxcps"] = lc_max_cps;
	data["lc_breakblocks"] = lc_break_blocks;
	data["lc_blockhit"] = lc_block_hit;
	data["lc_blockhitms"] = lc_block_hit_ms;
	data["lc_blockhitpause"] = lc_block_hit_stop_lclick;
	data["lc_smartcps"] = lc_targeting_affects_cps;
	data["lc_check"] = lc_click_check;
	data["lc_weaponsonly"] = lc_weapons_only;
	data["lc_tradeassist"] = lc_trade_assist;
	data["lc_bbStart"] = lc_start_break_blocks_reaction;
	data["lc_bbStop"] = lc_stop_break_blocks_reaction;

	// right auto clicker
	data["rc_enabled"] = rc_enabled;
	data["rc_cps"] = rc_cps;
	data["rc_blocksonly"] = rc_blocks_only;
	data["rc_startdelay"] = rc_start_delayms;
	data["rc_check"] = rc_click_check;

	// aim assist
	data["aa_enabled"] = aa_enabled;
	data["aa_distance"] = aa_distance;
	data["aa_speed"] = aa_speed;
	data["aa_horizontal_only"] = aa_horizontal_only;
	data["aa_fov"] = aa_fov;
	data["aa_invisibles"] = aa_invisibles;
	data["aa_mode"] = aa_target_mode;
	data["aa_always_aim"] = aa_always_aim;
	data["aa_multipoint"] = aa_aim_at_closest_point;
	data["aa_lockaim"] = aa_lock_aim;
	data["aa_bb"] = aa_break_blocks;

	// no click delay
	data["ncd_enabled"] = ncd_enabled;

	// bridge assist
	data["ba_enabled"] = ba_enabled;
	data["ba_pitch_check"] = ba_pitch_check;
	data["ba_block_check"] = ba_block_check;
	data["ba_sneak"] = ba_only_initiate_when_sneaking;

	// chest stealer
	data["cs_enabled"] = cs_enabled;
	data["cs_delay"] = cs_average_slowness_ms;
	data["cs_items"] = cs_items_to_grab;
	data["cs_key"] = cs_steal_key;
	data["cs_show_slot_pos"] = cs_show_slot_positions;

	json chest_stealer_info;

	for (const auto& i : cs_slot_info)
	{
		json info;
		info["beginx"] = i.begin_x;
		info["beginy"] = i.begin_y;
		info["spacex"] = i.space_x;
		info["spacey"] = i.space_y;
		info["resx"] = i.res_x;
		info["resy"] = i.res_y;

		chest_stealer_info.emplace_back(info);
	}

	data["cs_slot_info"] = chest_stealer_info;

	// blink
	data["bl_enabled"] = bl_enabled;
	data["bl_key"] = bl_hold_key;
	data["bl_stop_incoming_packets"] = bl_stop_rec_packets;
	data["bl_show_trail"] = bl_show_trail;
	data["bl_limit_seconds"] = bl_limit_seconds;

	// velocity
	data["vel_enabled"] = vel_enabled;
	data["vel_onlyclicking"] = vel_only_when_clicking;
	data["vel_onlymoving"] = vel_only_when_moving;
	data["vel_kite"] = vel_kite;
	data["vel_jumpreset"] = vel_jump_reset;
	data["vel_jumpchance"] = vel_jump_press_chance;
	data["vel_horizontal"] = vel_horizontal;
	data["vel_vertical"] = vel_vertical;
	data["vel_chance"] = vel_chance;
	data["vel_delay"] = vel_delay;

	// ui
	data["ui_list"] = ui_show_array_list;
	data["ui_mark"] = ui_show_water_mark;

	// esp
	data["esp_enabled"] = esp_enabled;
	data["esp_linecolr"] = esp_line_col[0];
	data["esp_linecolg"] = esp_line_col[1];
	data["esp_linecolb"] = esp_line_col[2];
	data["esp_linecola"] = esp_line_col[3];
	data["esp_fillcolr"] = esp_fill_col[0];
	data["esp_fillcolg"] = esp_fill_col[1];
	data["esp_fillcolb"] = esp_fill_col[2];
	data["esp_fillcola"] = esp_fill_col[3];
	data["esp_show_name"] = esp_show_name;
	data["esp_show_distance"] = esp_show_distance;
	data["esp_show_health"] = esp_show_health;
	data["esp_show_sneak"] = esp_show_sneaking;
	data["esp_mode"] = esp_esp_mode;
	data["esp_bg"] = esp_show_txt_bg;
	// esp extra
	data["esp_static_width"] = esp_static_esp_width;
	data["esp_text_shadow"] = esp_text_shadow;
	data["esp_text_colr"] = esp_text_col[0];
	data["esp_text_colg"] = esp_text_col[1];
	data["esp_text_colb"] = esp_text_col[2];
	data["esp_text_cola"] = esp_text_col[3];
	data["esp_bgcolr"] = esp_text_bg_col[0];
	data["esp_bgcolg"] = esp_text_bg_col[1];
	data["esp_bgcolb"] = esp_text_bg_col[2];
	data["esp_bgcola"] = esp_text_bg_col[3];
	data["esp_fontsize"] = esp_text_size;
	data["esp_border"] = esp_show_border;

	// block esp
	data["blockesp_enabled"] = besp_enabled;
	json blockArray = json::object();
	for (const auto& [id, col] : besp_block_list)
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

	if (include_keybinds)
	{
		data["lc_key"] = lc_key;
		data["rc_key"] = rc_key;
		data["aa_key"] = aa_key;
		data["ncd_key"] = ncd_key;
		data["ba_key"] = ba_key;
		data["cs_key"] = cs_key;
		data["bl_key"] = bl_key;
		data["vel_key"] = vel_key;
		data["ui_key"] = ui_show_array_list_key;
		data["esp_key"] = esp_key;
	}

	return data;
}

void Config::LoadConfig(std::string_view path, std::string_view file_name, std::string_view file_ext)
{
	std::ifstream f;
	char fullPath[MAX_PATH] = {};

	loaded_config = file_name;

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

		std::string err;
		if (!LoadSettings(ssbuf.str(), err))
		{
			std::cout << err << std::endl;
		}

	}
	else
	{
		std::cout << "failed to open file with path: " << fullPath << std::endl;
	}

}

void Config::SaveConfig(std::string_view path, std::string_view file_name, bool save_keybinds, std::string_view file_ext)
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
		f << SettingsToJson(save_keybinds).dump();
	}
	else if (f.fail())
	{
		std::cout << "Failed to save config to " << fullPath << std::endl;
	}
}

bool Config::LoadConfigFromClipBoard()
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

	std::string err;
	LoadSettings(data, err);

	return true;
}

void Config::SaveConfigToClipBoard()
{
	std::stringstream ss;
	ss << SettingsToJson().dump();

	size_t n = ss.str().length() + 1;

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

std::vector<ConfigFile> Config::GetAllConfigsInDirectory(std::string_view path)
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