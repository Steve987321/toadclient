#pragma once

// global settings for the modules are stored here

namespace toad
{
	namespace clicker
	{
		inline bool enabled = false;
		inline bool item_whitelist = false;
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

	namespace velocity
	{
		inline bool enabled = false;

		inline bool use_item_whitelist;

		inline bool only_when_moving = false;
		inline bool only_when_attacking = false;

		inline int chance = 100;
		inline float delay = 0;

		// in % the lower the less vel
		inline float horizontal = 100;
		inline float vertical = 100;
	}

	namespace EntityEsp
	{
		inline bool enabled = false;
	}

	namespace auto_bridge
	{
		inline bool enabled = false;
		inline float pitch_check = 61.f;
	}
}