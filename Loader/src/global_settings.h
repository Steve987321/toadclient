#pragma once

///
/// global settings for the modules are stored here
/// this file is also included inside the dll project
///

#include <unordered_map>

#include "imgui/imgui.h"

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

	inline std::unordered_map<ESP_MODE, const char*> espModeToCStrMap =
	{
		{ESP_MODE::BOX3D, "Box 3D"},
		{ESP_MODE::BOX2D_STATIC, "Static Box 2D"},
		{ESP_MODE::BOX2D_DYNAMIC, "Dynamic Box 2D"},
	};

	inline MC_CLIENT g_curr_client = MC_CLIENT::NOT_UPDATED;
	inline bool g_is_ui_internal = false;

	namespace left_clicker
	{
		inline bool enabled = false;

		inline int cps = 15;

		//inline bool item_whitelist = false;
		inline bool weapons_only = false; // only click when holding weapon
		inline bool break_blocks = false; // will hold down lmb when aiming at block
		inline bool trade_assist = false; // when trading hits cps spikes
		inline bool targeting_affects_cps = false; // when aiming at target cps is higher, else it lowers
		inline bool block_hit = false; // when hitting player it blocks for ms, see @block_hit_ms
		inline int block_hit_ms = 10; 

	}

	namespace right_clicker
	{
		inline bool enabled = false;
		inline int cps = 15;

		inline int start_delayms = 10;
		inline bool blocks_only = false;
	}

	namespace aa
	{
		inline bool enabled = false;
		inline bool use_item_whitelist = false;
		inline bool horizontal_only = false;
		inline bool invisibles = false;
		inline bool targetFOV = false;
		inline bool always_aim = false;
		inline bool break_blocks = false; // when breaking a block it will stop aa
		inline bool aim_at_closest_point = false;
		inline bool lock_aim = false; // locks the aim to a target until mouse is released for a short time

		inline int fov = 180.f;

		inline float distance = 5.f;
		inline float speed = 50.f;
		inline float reaction_time = 10; //ms
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

		inline bool jump_reset; // uses jump reset instead, will ignore everything else and justs jumps on hit

		inline bool only_when_moving = false;
		inline bool only_when_attacking = false;

		inline int chance = 100;
		inline float delay = 0;

		// in % the lower the less vel
		inline float horizontal = 100;
		inline float vertical = 100;
	}

	namespace esp
	{
		inline bool enabled = false;
		inline float line_col[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		inline float fill_col[4] = {1.0f, 1.0f, 1.0f, 0.3f};

		inline bool show_distance = false;
		inline bool show_name = false;
		inline bool show_health = false;

		inline ESP_MODE esp_mode;

		inline int line_width = 1; // pixels
		inline bool text_shadow = false; // use a text outline
		inline bool text_bg = false; // background for text
		inline float text_col[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		inline float text_bg_col[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
		inline float static_esp_width = 0;

		/*inline const char* font_path = "";
		inline int font_size = 15;*/

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
	}

	namespace blink
	{
		inline bool enabled = false;
		inline int key = 0; // key to be pressed/held to blink
		inline float limit_seconds = 5.f; // max limit in seconds for blink to be enabled
		inline bool stop_rec_packets = false; // also stops/pauses incoming packets
		inline bool show_trail = false; // renders a trail from the position when enabled to current position
	}
}
