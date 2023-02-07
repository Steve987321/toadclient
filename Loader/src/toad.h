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
	
	[[nodiscard]] bool pre_init();

	[[nodiscard]] bool init();

	void update_settings();
	inline std::thread tupdate_settings;

	// stop all threads that are currently running 
	void stop_all_threads();

	void clean_up();

	// will be true when the program is in its main loop 
	inline std::atomic_bool is_running = false;

	// will be true when injection was succesfull
	inline bool is_verified = false;

	namespace aa
	{
		inline bool enabled = false;
		inline float distance = 5.f;
		inline float speed = 5.f;
	}

}
