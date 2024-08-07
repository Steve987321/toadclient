#ifdef TOAD_LOADER
#include "config.h"
#include "toad.h"
#endif
#include <filesystem>
#include <fstream>

namespace config
{

using json = nlohmann::json;

bool LoadSettings(std::string_view jsonSettings, std::string& error_msg)
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

	try
	{
		g_is_ui_internal = data.at("ui_internal");

		// left auto clicker
		get_json_element(left_clicker::enabled, data, "lc_enabled");
		get_json_element(left_clicker::enabled, data, "lc_enabled");
		get_json_element(left_clicker::min_cps, data, "lc_mincps");
		get_json_element(left_clicker::max_cps, data, "lc_maxcps");
		get_json_element(left_clicker::break_blocks, data, "lc_breakblocks");
		get_json_element(left_clicker::block_hit, data, "lc_blockhit");
		get_json_element(left_clicker::block_hit_ms, data, "lc_blockhitms");
		get_json_element(left_clicker::block_hit_stop_lclick, data, "lc_blockhitpause");
		get_json_element(left_clicker::targeting_affects_cps, data, "lc_smartcps");
		get_json_element(left_clicker::click_check, data, "lc_check");
		get_json_element(left_clicker::weapons_only, data, "lc_weaponsonly");
		get_json_element(left_clicker::trade_assist, data, "lc_tradeassist");
		get_json_element(left_clicker::start_break_blocks_reaction, data, "lc_bbStart");
		get_json_element(left_clicker::stop_break_blocks_reaction, data, "lc_bbStop");

		// right auto clicker
		get_json_element(right_clicker::enabled, data, "rc_enabled");
		get_json_element(right_clicker::cps, data, "rc_cps");
		get_json_element(right_clicker::blocks_only, data, "rc_blocksonly");
		get_json_element(right_clicker::start_delayms, data, "rc_startdelay");
		get_json_element(right_clicker::click_check, data, "rc_check");

		// rand 
		json lcRandBoosts;
		json lcRandInconsistencies;
		json lcRandInconsistencies2;

		//json lcRandBoosts = data.at("lc_randb");
		get_json_element(lcRandBoosts, data, "lc_randb");
		get_json_element(lcRandInconsistencies, data, "lc_randi");
		get_json_element(lcRandInconsistencies2, data, "lc_randi2");
		//json lcRandInconsistencies = data.at("lc_randi");
		//json lcRandInconsistencies2 = data.at("lc_randi2");

		left_clicker::rand.boosts.clear();
		left_clicker::rand.inconsistencies.clear();
		left_clicker::rand.inconsistencies2.clear();

		for (auto& item : lcRandBoosts.items())
		{
			int id = std::stoi(item.key());
			float amount_ms = item.value().at("n");
			float duration = item.value().at("dur");
			float transition_duration = item.value().at("tdur");
			float freq_min = item.value().at("fqmin");
			float freq_max = item.value().at("fqmax");
			left_clicker::rand.boosts.emplace_back(amount_ms, duration, transition_duration, freq_min, freq_max, id);
		}

		for (auto& item : lcRandInconsistencies.items())
		{
			float min_amount_ms = item.value().at("nmin");
			float max_amount_ms = item.value().at("nmax");
			float chance = item.value().at("c");
			float frequency = item.value().at("f");
			left_clicker::rand.inconsistencies.emplace_back(min_amount_ms, max_amount_ms, chance, frequency);
		}

		for (auto& item : lcRandInconsistencies2.items())
		{
			float min_amount_ms = item.value().at("nmin");
			float max_amount_ms = item.value().at("nmax");
			float chance = item.value().at("c");
			float frequency = item.value().at("f");
			left_clicker::rand.inconsistencies2.emplace_back(min_amount_ms, max_amount_ms, chance, frequency);
		}

		left_clicker::update_rand_flag = true;

		// aim assist
		get_json_element(aa::enabled, data, "aa_enabled");
		get_json_element(aa::distance, data, "aa_distance");
		get_json_element(aa::speed, data, "aa_speed");
		get_json_element(aa::horizontal_only, data, "aa_horizontal_only");
		get_json_element(aa::fov, data, "aa_fov");
		get_json_element(aa::invisibles, data, "aa_invisibles");
		get_json_element(aa::target_mode, data, "aa_mode");
		get_json_element(aa::always_aim, data, "aa_always_aim");
		get_json_element(aa::aim_at_closest_point, data, "aa_multipoint");
		get_json_element(aa::lock_aim, data, "aa_lockaim");
		get_json_element(aa::break_blocks, data, "aa_bb");

		// no click delay
		get_json_element(no_click_delay::enabled, data, "ncd_enabled");

		// bridge assist
		get_json_element(bridge_assist::enabled, data, "ba_enabled");
		get_json_element(bridge_assist::pitch_check, data, "ba_pitch_check");
		get_json_element(bridge_assist::block_check, data, "ba_block_check");
		get_json_element(bridge_assist::only_initiate_when_sneaking, data, "ba_sneak");

		// chest stealer
		get_json_element(chest_stealer::enabled, data, "cs_enabled");
		get_json_element(chest_stealer::average_slowness_ms, data, "cs_delay");
		get_json_element(chest_stealer::items_to_grab, data, "cs_items");
		get_json_element(chest_stealer::steal_key, data, "cs_key");
		get_json_element(chest_stealer::show_slot_positions, data, "cs_show_slot_pos");

		json slot_info;
		get_json_element(slot_info, data, "cs_slot_info");
		if (slot_info.size() != chest_stealer::slot_info.size())
		{
			std::cout << "Resize slot info to " << slot_info.size() << std::endl;
			chest_stealer::slot_info.resize(slot_info.size());
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

			chest_stealer::slot_info[index++] = std::move(info);
		}

		// blink
		get_json_element(blink::enabled, data, "bl_enabled");
		get_json_element(blink::key, data, "bl_key");
		get_json_element(blink::stop_rec_packets, data, "bl_stop_incoming_packets");
		get_json_element(blink::show_trail, data, "bl_show_trail");
		get_json_element(blink::limit_seconds, data, "bl_limit_seconds");

		// velocity
		get_json_element(velocity::enabled, data, "vel_enabled");
		get_json_element(velocity::jump_reset, data, "vel_jumpreset");
		get_json_element(velocity::horizontal, data, "vel_horizontal");
		get_json_element(velocity::vertical, data, "vel_vertical");
		get_json_element(velocity::chance, data, "vel_chance");
		get_json_element(velocity::delay, data, "vel_delay");

		get_json_element(velocity::enabled, data, "vel_enabled");
		get_json_element(velocity::only_when_clicking, data, "vel_onlyclicking");
		get_json_element(velocity::only_when_moving, data, "vel_onlymoving");
		get_json_element(velocity::kite, data, "vel_kite");
		get_json_element(velocity::jump_reset, data, "vel_jumpreset");
		get_json_element(velocity::jump_press_chance, data, "vel_jumpchance");
		get_json_element(velocity::horizontal, data, "vel_horizontal");
		get_json_element(velocity::vertical, data, "vel_vertical");
		get_json_element(velocity::chance, data, "vel_chance");
		get_json_element(velocity::delay, data, "vel_delay");

		// ui
		get_json_element(ui::show_array_list, data, "ui_list");
		get_json_element(ui::show_water_mark, data, "ui_mark");

		// esp
		get_json_element(esp::enabled, data, "esp_enabled");
		get_json_element(esp::line_col[0], data, "esp_linecolr");
		get_json_element(esp::line_col[1], data, "esp_linecolg");
		get_json_element(esp::line_col[2], data, "esp_linecolb");
		get_json_element(esp::line_col[3], data, "esp_linecola");
		get_json_element(esp::fill_col[0], data, "esp_fillcolr");
		get_json_element(esp::fill_col[1], data, "esp_fillcolg");
		get_json_element(esp::fill_col[2], data, "esp_fillcolb");
		get_json_element(esp::fill_col[3], data, "esp_fillcola");
		get_json_element(esp::text_bg_col[0], data, "esp_bgcolr");
		get_json_element(esp::text_bg_col[1], data, "esp_bgcolg");
		get_json_element(esp::text_bg_col[2], data, "esp_bgcolb");
		get_json_element(esp::text_bg_col[3], data, "esp_bgcola");
		get_json_element(esp::show_name, data, "esp_show_name");
		get_json_element(esp::show_distance, data, "esp_show_distance");
		get_json_element(esp::show_health, data, "esp_show_health");
		get_json_element(esp::show_sneaking, data, "esp_show_sneak");
		get_json_element(esp::esp_mode, data, "esp_mode");
		get_json_element(esp::show_txt_bg, data, "esp_bg");

		// esp extra
		get_json_element(esp::static_esp_width, data, "esp_static_width");
		get_json_element(esp::text_shadow, data, "esp_text_shadow");
		get_json_element(esp::text_col[0], data, "esp_text_colr");
		get_json_element(esp::text_col[1], data, "esp_text_colg");
		get_json_element(esp::text_col[2], data, "esp_text_colb");
		get_json_element(esp::text_col[3], data, "esp_text_cola");
		get_json_element(esp::text_size, data, "esp_fontsize");
		get_json_element(esp::show_border, data, "esp_border");

		// block esp
		json block_array;

		get_json_element(block_esp::enabled, data, "blockesp_enabled");
		if (get_json_element(block_array, data, "block_esp_array"))
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

			block_esp::block_list = tmpList;
		}
	}
	catch (json::out_of_range& e)
	{
		error_msg = "error getting settings: " + std::string(e.what());
		return false;
	}
	catch (json::type_error& e)
	{
		error_msg = "type error: " + std::string(e.what());
		return false;
	}
	catch(std::exception& e)
	{ 
		error_msg = "exception: " + std::string(e.what());
		return false;
	}

	return true;
}

json MergeJson(const json& a, const json& b)
{
	json result = a.flatten();
	json tmp = b.flatten();

	for (json::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		result[it.key()] = it.value();
	}

	return result.unflatten();
}

json SettingsToJson()
{
	// settings 
	json data;

	// rand
	json lcRandInconsistencies = json::object();
	json lcRandInconsistencies2 = json::object();
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
		lcRandInconsistencies[std::to_string(i)] =
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
	data["lc_enabled"] = left_clicker::enabled;
	data["lc_mincps"] = left_clicker::min_cps;
	data["lc_maxcps"] = left_clicker::max_cps;
	data["lc_breakblocks"] = left_clicker::break_blocks;
	data["lc_blockhit"] = left_clicker::block_hit;
	data["lc_blockhitms"] = left_clicker::block_hit_ms;
	data["lc_blockhitpause"] = left_clicker::block_hit_stop_lclick;
	data["lc_smartcps"] = left_clicker::targeting_affects_cps;
	data["lc_check"] = left_clicker::click_check;
	data["lc_weaponsonly"] = left_clicker::weapons_only;
	data["lc_tradeassist"] = left_clicker::trade_assist;
	data["lc_bbStart"] = left_clicker::start_break_blocks_reaction;
	data["lc_bbStop"] = left_clicker::stop_break_blocks_reaction;

	// right auto clicker
	data["rc_enabled"] = right_clicker::enabled;
	data["rc_cps"] = right_clicker::cps;
	data["rc_blocksonly"] = right_clicker::blocks_only;
	data["rc_startdelay"] = right_clicker::start_delayms;
	data["rc_check"] = right_clicker::click_check;

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
	data["ba_sneak"] = bridge_assist::only_initiate_when_sneaking;

	// chest stealer
	data["cs_enabled"] = chest_stealer::enabled;
	data["cs_delay"] = chest_stealer::average_slowness_ms;
	data["cs_items"] = chest_stealer::items_to_grab;
	data["cs_key"] = chest_stealer::steal_key;
	data["cs_show_slot_pos"] = chest_stealer::show_slot_positions;

	json chest_stealer_info;

	for (const auto& i : chest_stealer::slot_info)
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

	// ui
	data["ui_list"] = ui::show_array_list;
	data["ui_mark"] = ui::show_water_mark;

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
	data["esp_show_sneak"] = esp::show_sneaking;
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

void LoadConfig(std::string_view path, std::string_view file_name, std::string_view file_ext)
{
	std::ifstream f;
	char fullPath[MAX_PATH] = {};

	toad::loaded_config = file_name;

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

	std::string err;
	LoadSettings(data, err);

	return true;

}

void SaveConfigToClipBoard()
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