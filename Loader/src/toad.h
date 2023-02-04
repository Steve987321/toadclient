#pragma once


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream> 
#include <tchar.h>
#include <thread> 
#include <vector>
#include <algorithm>
#include <string>

#include "utils/utils.h"

#define SLOW_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

namespace toad
{
	[[nodiscard]] bool init_toad();

	// stop all threads that are running 
	void stop_all_threads();

	// important
	inline std::atomic_bool is_running = false;

	inline bool is_verified = false;

}
