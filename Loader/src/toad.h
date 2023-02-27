#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream> 
#include <tchar.h>
#include <thread> 
#include <vector>
#include <algorithm>
#include <string>
#include <TlHelp32.h>
#include <sstream>
#include <shared_mutex>

// TOAD
#include "utils/utils.h"

// use this when precision isn't required but the CPU should be saved
#define SLOW_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

namespace toad
{
	inline bool dll_debug_mode = false;

	// before init
	[[nodiscard]] bool pre_init();

	[[nodiscard]] bool init();

	void Fupdate_settings();
	inline std::thread Tupdate_settings;

	// stop all threads that are currently running 
	void stop_all_threads();

	void clean_up();

	// will be true when the program is in its main loop 
	inline std::atomic_bool is_running = false;

	// will be true when injection was succesfull
	inline bool is_verified = false;

	namespace clicker
	{
		inline bool enabled = false;
		inline bool item_whitelist = false;
	}

	namespace aa
	{
		inline bool enabled = false;
		inline bool use_item_whitelist = false;
		inline utils::mc_items mc_items_whitelist;
		inline bool horizontal_only = false;
		inline bool invisibles = false;
		inline bool targetFOV = false;
		inline bool always_aim = false;

		inline int fov = 180.f;

		inline float distance = 5.f;
		inline float speed = 5.f;
	}

	namespace velocity
	{
		inline bool enabled = false;

		inline bool use_item_whitelist;
		inline utils::mc_items mc_items_whitelist;

		inline bool only_when_moving = false;
		inline bool only_when_attacking = false;

		inline int chance = 100;
		inline float delay = 0;

		// in % the lower the less vel
		inline float horizontal = 100;
		inline float vertical = 100;
	}


	namespace auto_bridge
	{
		inline bool enabled = false;
		inline float pitch_check = 61.f;
	}

}
