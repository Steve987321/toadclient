#pragma once

#include "imgui/imgui.h"
#include "Toad/Modules/clicker/rand_types.h"
#include "nlohmann/json.hpp"

namespace toad
{
	struct ConfigFile
	{
		std::string FileNameStem;
		std::filesystem::file_time_type LastWrite;
	};

	enum class MC_CLIENT
	{
		Lunar_189,
		Lunar_171,
		NOT_SUPPORTED,
	};

	enum class CLICK_CHECK
	{
		ONLY_INVENTORY,
		ONLY_GAME,
		ALWAYS
	};

	enum class ESP_MODE
	{
		BOX3D,
		BOX2D_STATIC,
		BOX2D_DYNAMIC
	};

	enum class ESP_HEALTH_MODE
	{
		BAR_DEFAULT,
		BAR_STATIC_GRADIENT,
		TEXT,
	};

	enum class AA_TARGET
	{
		HEALTH,
		DISTANCE,
		FOV
	};

	inline std::unordered_map<ESP_MODE, const char*> espModeToCStrMap =
	{
		{ESP_MODE::BOX3D, "Box 3D"},
		{ESP_MODE::BOX2D_STATIC, "Static Box 2D"},
		{ESP_MODE::BOX2D_DYNAMIC, "Dynamic Box 2D"},
	};

	inline std::unordered_map<AA_TARGET, const char*> AATargetToCStrMap =
	{
		{AA_TARGET::DISTANCE, "Closest to Player"},
		{AA_TARGET::HEALTH, "Lowest Health"},
		{AA_TARGET::FOV, "Closest to Crosshair"},
	};

	inline std::unordered_map<CLICK_CHECK, const char*> clickCheckToCStrMap =
	{
		{CLICK_CHECK::ONLY_INVENTORY, "Only in inventory"},
		{CLICK_CHECK::ONLY_GAME, "Only in game"},
		{CLICK_CHECK::ALWAYS, "Always click"},
	};

	struct ChestStealerSlotLocationInfo
	{
		int res_x = -1; 
		int res_y = -1;

		int begin_x = -135;
		int begin_y = -70;

		int space_x = 35;
		int space_y = 35;
	};

	class TOAD_API Config
	{
	public:
		static Config& Get();

		void RegisterConfigVars();

		/// Sets the current settings of a json string 
		bool LoadSettings(std::string_view jsonSettings, std::string& error_msg);

		/// Returns the combined data of a and b 
		nlohmann::json MergeJson(const nlohmann::json& a, const nlohmann::json& b);

		/// Returns a json of the current settings
		nlohmann::json SettingsToJson(bool include_keybinds = true);

		/// Load a config from a file
		///
		///	@param path the directory the file is in
		///	@param file_name the name of the file
		///	@param file_ext the extension of the file
		void LoadConfig(std::string_view path, std::string_view file_name, std::string_view file_ext = ".toad");

		/// Save the current settings to a file
		///
		///	@param path the directory to save to
		///	@param file_name the name of the file
		/// @param save_keybinds whether to save keybinds to file
		///	@param file_ext the extension of the file 
		void SaveConfig(std::string_view path, std::string_view file_name, bool save_keybinds = true, std::string_view file_ext = ".toad");

		/// Load a config from the clipboard 
		bool LoadConfigFromClipBoard();

		/// Save the current settings to the clipboard
		void SaveConfigToClipBoard();

		/// Returns a list of all possible configs for toad
		///
		///	@param path the directory we look in
		static std::vector<ConfigFile> GetAllConfigsInDirectory(std::string_view path);

		MC_CLIENT g_curr_client = MC_CLIENT::NOT_SUPPORTED;
		bool g_is_ui_internal = false;

		std::string loader_path;
		std::string loaded_config;

		uint32_t ipc_bufsize = 10000;

		// if we want to update the rand to the internal clicker 
		bool lc_update_rand_flag = false;
		bool lc_enabled = false;
		int lc_key = 0;
		// base rand
		float lc_min_cps = 12;
		float lc_max_cps = 16;
		//bool item_whitelist = false;
		bool lc_weapons_only = false; // only click when holding weapon
		bool lc_break_blocks = false; // will hold down lmb when aiming at block
		bool lc_trade_assist = false; // when trading hits cps spikes
		bool lc_targeting_affects_cps = false; // when aiming at target cps is higher, else it lowers
		CLICK_CHECK lc_click_check = CLICK_CHECK::ONLY_GAME; // checks when clicking should happen
		bool lc_block_hit = false; // when hitting player it blocks for ms, see @block_hit_ms
		int lc_block_hit_ms = 50; // rmb hold time 
		bool lc_block_hit_stop_lclick = false; // pauses the left clicker while holding the rmb
		int lc_start_break_blocks_reaction = 60; // reaction time to start breaking blocks
		int lc_stop_break_blocks_reaction = 60; // reaction time to stop breaking blocks

		toad::Randomization lc_rand = toad::Randomization(
			0,
			0,
			20,
			50,
			0,
			0,
			{
				toad::Inconsistency(10.f, 40.f , 70, 35),
				toad::Inconsistency(20.f, 40.f , 60, 50),
				toad::Inconsistency(40.f, 60.f, 50, 150),

				toad::Inconsistency(-10.f, 0    , 50, 40),
			},
			{
				toad::Inconsistency(30.f, 50.f , 70, 50),
				toad::Inconsistency(60.f, 80.f , 50, 100),

				toad::Inconsistency(-10.f, 0, 60, 50),
				toad::Inconsistency(-15.f, 0, 40, 60)
			},
			{
				toad::Boost(1.2f, 50 , 3, 100, 150, 0),
				toad::Boost(0.5f, 80 , 5, 100, 150, 1),
				toad::Boost(0.4f, 100, 5, 150, 200, 2),
				toad::Boost(1.0f, 120, 5, 150, 200, 3),

				// DROPS
			toad::Boost(-1.0f, 190, 5, 150, 200, 4),
			toad::Boost(-1.5f, 50, 3, 100, 200, 5),

			}
			);

		bool rc_enabled = false;
		int rc_key = 0;
		int rc_cps = 15;
		CLICK_CHECK rc_click_check = CLICK_CHECK::ONLY_GAME; // checks when clicking should happen
		int rc_start_delayms = 10;
		bool rc_blocks_only = false;

		bool aa_enabled = false;
		int aa_key = 0;
		bool aa_use_item_whitelist = false;
		bool aa_horizontal_only = false;
		bool aa_invisibles = false;
		bool aa_always_aim = false;
		bool aa_break_blocks = false; // when breaking a block it will stop aa
		bool aa_aim_at_closest_point = false;
		bool aa_lock_aim = false; // locks the aim to a target until mouse is released for a short time
		AA_TARGET aa_target_mode = AA_TARGET::DISTANCE;
		int aa_fov = 180;
		float aa_distance = 5.f;
		float aa_speed = 50.f;
		int aa_reaction_time = 50; // ms

		bool vel_enabled = false;
		int vel_key = 0;
		bool vel_jump_reset = false; // uses jump reset instead, will ignore everything else and justs jumps on hit
		int vel_jump_press_chance = 70;
		bool vel_only_when_moving = false;
		bool vel_only_when_clicking = false;
		bool vel_kite = false; // don't decrease velocity when hit from behind
		int vel_chance = 100;
		int vel_delay = 0;
		// in % the lower the less vel
		float vel_horizontal = 100;
		float vel_vertical = 100;

		bool esp_enabled = false;
		int esp_key = 0;
		float esp_line_col[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		float esp_fill_col[4] = { 1.0f, 1.0f, 1.0f, 0.1f };
		bool esp_show_distance = false;
		bool esp_show_name = false;
		bool esp_show_health = false;
		bool esp_show_sneaking = false;
		ESP_MODE esp_esp_mode;
		bool esp_show_border = false; // enable border outlines for all boxes
		bool esp_text_shadow = false; // use a text outline
		bool esp_show_txt_bg = false; // background for text
		float esp_text_col[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float esp_text_bg_col[4] = { 0.1f, 0.1f, 0.1f, 0.45f };
		float esp_text_size = 13.f;
		int esp_static_esp_width = 0;
		std::string esp_font_path = "Default";
		bool esp_update_font_flag = false;

		bool ncd_enabled = false;
		int ncd_key = 0;

		bool besp_enabled = false;
		int besp_key = 0;
		std::unordered_map<int, ImVec4> besp_block_list;

		bool ba_enabled = false;
		int ba_key = 0;
		float ba_pitch_check = 61.f; // only sneak when pitch is less
		int ba_block_check = 1; // only sneak when edge height is bigger then this value in blocks
		bool ba_only_initiate_when_sneaking = false; // only start bridge assisting when holding sneak in the beginning

		bool bl_enabled = false;
		int bl_key = 0;
		int bl_hold_key = 0; // key to be pressed/held to blink
		float bl_limit_seconds = 5.f; // max limit in seconds for blink to be enabled
		bool bl_stop_rec_packets = false; // also stops/pauses incoming packets
		bool bl_show_trail = false; // renders a trail from the position when enabled to current position

		bool cs_enabled = false;
		int cs_key = 0;
		int cs_steal_key = 0;
		bool cs_show_slot_positions = false; // shows helper grid for setting the offsets
		bool cs_show_info = false;
		std::vector<std::string> cs_items_to_grab{};
		std::vector<ChestStealerSlotLocationInfo> cs_slot_info{};
		int cs_average_slowness_ms = 80; // average speed between pickup
		int cs_missclick_chance = 50; // chance of missing the item (will still go back and pick it up)

		// for internal ui 
		int ui_show_menu_key = 0x2D; // INSERT
		// show enabled modules
		bool ui_show_array_list = false;
		int ui_show_array_list_key = 0;
		int ui_array_list_size = 20;
		bool ui_show_water_mark = false;
	};
}
