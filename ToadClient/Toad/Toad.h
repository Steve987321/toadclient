#pragma once

#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

#include "Toad/Types.h"
#include "Toad/Logger.h"

#include "helpers.h"

#include "Toad/MC/Utils/utils.h"
#include "Toad/MC/mappings.h"
#include "Toad/MC/Entity/Entity.h"
#include "Toad/MC/Minecraft.h"

#include "Toad/Modules/modules.h"

#include "MinHook/include/MinHook.h"
#pragma comment(lib, "minhook.x64.lib")
#include "Hooks/Hooks.h"

// use this when precision isn't required but the CPU should be saved
#define SLOW_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

// global vars and functions 
namespace toadll
{
	inline minecraft_client curr_client = minecraft_client::Lunar;

	inline std::atomic_bool is_running = false;

	inline std::thread Tupdate_settings;
	inline std::thread Tupdate_cursorinfo;
	inline std::thread Tupdate_hookvars;

	inline bool is_cursor_shown = false;

	inline HMODULE hMod;

	inline JNIEnv* env = nullptr;
	inline JavaVM* jvm = nullptr;

	namespace aa
	{
		inline bool enabled = false;
		inline int key = VK_LBUTTON;

		inline bool targetFOV = false;
		inline bool invisibles = false;
		inline bool horizontal_only = false;
		inline bool always_aim = false;

		inline float reaction_speed = 100.f; // reaction in ms
		inline float speed = 5.0f;
		inline float distance = 4.0f;
		inline int fov = 360;
	}

	namespace auto_bridge
	{
		inline bool enabled = false;
		inline float pitch_check = 61;
	}

	// called when dll has injected
	DWORD WINAPI init();

	// the main loop when init was succesfull
	void update();

	// called when wanting to uninject and cleans up
	void clean_up(int exitcode = 0);
}