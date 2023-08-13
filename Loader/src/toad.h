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
#include <queue>

// TOAD
#include "global_settings.h"
#include "utils/helpers.h"
#include "utils/block_map.h"
#include "Application/Application.h"
#include "Application/ui.h"
#include "Application/VisualizeClicker/VisualizeClicker.h"

// use this when precision isn't required but the CPU should be saved
#define SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

namespace toad
{
	// before init
	_NODISCARD bool pre_init();

	// Sets up ipc and threads and returns false if failed
	_NODISCARD bool init();

	// stop all threads that are currently running 
	void stop_all_threads();

	void clean_up();

	// will be true when the program is in its main loop 
	inline std::atomic_bool g_is_running = false;

	// only updated when still in init screen
	// contains the list of all minecraft windows that user can inject to  
	inline std::vector<window> g_mc_window_list = {};

	// will be true when injection was succesfull
	// will remain false if we haven't injected 
	inline bool g_is_verified = false;
}
