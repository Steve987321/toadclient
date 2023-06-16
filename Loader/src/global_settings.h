#pragma once

// global settings for the modules are stored here

#include <unordered_map>

#include "imgui/imgui.h"

namespace toad
{
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
		inline bool aim_at_closest_point = false;
		inline bool lock_aim = false; // locks the aim to a player until mouse is released for a short time

		inline int fov = 180.f;

		inline float distance = 5.f;
		inline float speed = 5.f;
		inline float reaction_time = 40; //ms
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
		inline float lineCol[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		inline float fillCol[4] = {1.0f, 1.0f, 1.0f, 0.3f};
	}

	namespace block_esp
	{
		inline bool enabled = false;
		inline std::unordered_map<int, ImVec4> block_list;
	}
	namespace auto_bridge
	{
		inline bool enabled = false;
		inline float pitch_check = 61.f;
	}
}
