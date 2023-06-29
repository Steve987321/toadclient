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
#include "global_settings.h"
#include "utils/utils.h"
#include "utils/block_map.h"
#include "Application/Application.h"
#include "Application/ui.h"

// use this when precision isn't required but the CPU should be saved
#define SLOW_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

namespace toad
{
	inline bool g_dll_debug_mode = false;

	// before init
	[[nodiscard]] bool pre_init();

	[[nodiscard]] bool init();

	void Fupdate_settings();
	inline std::thread Tupdate_settings;

	// stop all threads that are currently running 
	void stop_all_threads();

	void clean_up();

	// will be true when the program is in its main loop 
	inline std::atomic_bool g_is_running = false;

	// will be true when injection was succesfull
	inline bool g_is_verified = false;
}
