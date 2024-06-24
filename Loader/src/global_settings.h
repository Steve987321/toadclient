#pragma once

///
/// global settings for the modules are stored here
/// this file is also included inside the dll project
///

#include <unordered_map>

#include <random>

#include "imgui/imgui.h"

#ifdef TOAD_LOADER
#include <chrono>
#include "ToadClient/src/Toad/Modules/clicker/rand_types.h"
#include "ToadClient/src/Toad/timer.h"
#else
#include "Toad/Modules/clicker/rand_types.h"
#endif

namespace toad
{
	enum class MC_CLIENT
	{
		NOT_UPDATED,
		NOT_SUPPORTED,
		Lunar_189,
		Lunar_171,
		Vanilla,
		Forge
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

	inline MC_CLIENT g_curr_client = MC_CLIENT::NOT_UPDATED;
	inline bool g_is_ui_internal = false;

	inline std::string loader_path;
	inline std::string loaded_config;

	namespace left_clicker
	{
		// if we want to update the rand to the internal clicker 
		inline bool update_rand_flag = false; 

		inline bool enabled = false;

		inline int min_cps = 12;
		inline int max_cps = 16;

		//inline bool item_whitelist = false;
		inline bool weapons_only = false; // only click when holding weapon
		inline bool break_blocks = false; // will hold down lmb when aiming at block
		inline bool trade_assist = false; // when trading hits cps spikes
		inline bool targeting_affects_cps = false; // when aiming at target cps is higher, else it lowers
		inline CLICK_CHECK click_check = CLICK_CHECK::ONLY_GAME; // checks when clicking should happen
		inline bool block_hit = false; // when hitting player it blocks for ms, see @block_hit_ms
		inline int block_hit_ms = 50; // rmb hold time 
		inline bool block_hit_stop_lclick = false; // pauses the left clicker while holding the rmb

		inline int start_break_blocks_reaction = 60; // reaction time to start breaking blocks
		inline int stop_break_blocks_reaction = 60; // reaction time to stop breaking blocks

		inline toadll::Randomization rand = toadll::Randomization(
			0,
			0,
			20,
			50,
			0,
			0,
			{
				toadll::Inconsistency(10.f, 40.f , 70, 35),
				toadll::Inconsistency(20.f, 40.f , 60, 50),
				toadll::Inconsistency(40.f, 60.f, 50, 150),

				toadll::Inconsistency(-10.f, 0    , 50, 40),
			},
			{
				toadll::Inconsistency(30.f, 50.f , 70, 50),
				toadll::Inconsistency(60.f, 80.f , 50, 100),

				toadll::Inconsistency(-10.f, 0, 60, 50),
				toadll::Inconsistency(-15.f, 0, 40, 60)
			},
			{
				toadll::Boost(1.2f, 50 , 3, 100, 150, 0),
				toadll::Boost(0.5f, 80 , 5, 100, 150, 1),
				toadll::Boost(0.4f, 100, 5, 150, 200, 2),
				toadll::Boost(1.0f, 120, 5, 150, 200, 3),

					// DROPS
				toadll::Boost(-1.0f, 190, 5, 150, 200, 4),
				toadll::Boost(-1.5f, 50, 3, 100, 200, 5),

			}
		);

	}

	namespace right_clicker
	{
		inline bool enabled = false;
		inline int cps = 15;

		inline CLICK_CHECK click_check = CLICK_CHECK::ONLY_GAME; // checks when clicking should happen

		inline int start_delayms = 10;
		inline bool blocks_only = false;
	}

	namespace aa
	{
		inline bool enabled = false;
		inline bool use_item_whitelist = false;
		inline bool horizontal_only = false;
		inline bool invisibles = false;
		inline bool always_aim = false;
		inline bool break_blocks = false; // when breaking a block it will stop aa
		inline bool aim_at_closest_point = false;
		inline bool lock_aim = false; // locks the aim to a target until mouse is released for a short time

		inline AA_TARGET target_mode = AA_TARGET::DISTANCE;
		inline int fov = 180;

		inline float distance = 5.f;
		inline float speed = 50.f;
		inline int reaction_time = 50; // ms
	}

	namespace reach
	{
		inline bool enabled = false;
		inline float min_value = 3.00f;
		inline float max_value = 3.50f;
		inline int chance = 50;
	}

	namespace velocity
	{
		inline bool enabled = false;

		inline bool jump_reset = false; // uses jump reset instead, will ignore everything else and justs jumps on hit
		inline int jump_press_chance = 70;

		inline bool only_when_moving = false;
		inline bool only_when_clicking = false;

		inline bool kite = false; // don't decrease velocity when hit from behind

		inline int chance = 100;
		inline float delay = 0;

		// in % the lower the less vel
		inline float horizontal = 100;
		inline float vertical = 100;
	}

	namespace esp
	{
		inline bool enabled = false;
		inline float line_col[4] = {1.0f, 0.0f, 0.0f, 1.0f};
		inline float fill_col[4] = {1.0f, 1.0f, 1.0f, 0.1f};

		inline bool show_distance = false;
		inline bool show_name = false;
		inline bool show_health = false;
		inline bool show_sneaking = false;

		inline ESP_MODE esp_mode;

		inline bool show_border = false; // enable border outlines for all boxes
		inline bool text_shadow = false; // use a text outline
		inline bool show_txt_bg = false; // background for text
		inline float text_col[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float text_bg_col[4] = { 0.1f, 0.1f, 0.1f, 0.45f };
		inline float text_size = 13.f;

		inline int static_esp_width = 0;

		inline std::string font_path = "Default";

		inline bool update_font_flag = false;
	}

	namespace no_click_delay
	{
		inline bool enabled = false;
	}

	namespace block_esp
	{
		inline bool enabled = false;
		inline std::unordered_map<int, ImVec4> block_list;
	}

	namespace bridge_assist
	{
		inline bool enabled = false;
		inline float pitch_check = 61.f; // only sneak when pitch is less
		inline int block_check = 1; // only sneak when edge height is bigger then this value in blocks

		inline bool only_initiate_when_sneaking = false; // only start bridge assisting when holding sneak in the beginning
	}

	namespace blink
	{
		inline bool enabled = false;
		inline int key = 0; // key to be pressed/held to blink
		inline float limit_seconds = 5.f; // max limit in seconds for blink to be enabled
		inline bool stop_rec_packets = false; // also stops/pauses incoming packets
		inline bool show_trail = false; // renders a trail from the position when enabled to current position
	}

	namespace chest_stealer
	{
		inline bool enabled = true;
	}

	namespace ui
	{
		inline bool show_array_list = false;

		inline int array_list_size = 20;

		inline bool show_water_mark = false;
	}
}
