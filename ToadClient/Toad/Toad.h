#pragma once

#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

// use this when precision isn't required but the CPU should be saved
#define SLOW_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

// macro that inherits from Singleton interface class
#define SET_MODULE_CLASS(T) : public toadll::Singleton<T>, public CModule

#include "Toad/Types.h"
#include "Toad/Logger.h"

#include "helpers.h"

#include "Toad/MC/Utils/mcutils.h"
#include "Toad/MC/mappings.h"
#include "Toad/MC/ActiveRenderInfo/ActiveRenderInfo.h"
#include "Toad/MC/Entity/Entity.h"
#include "Toad/MC/Minecraft.h"

#include "singleton.h"
#include "module.h"

#include "Toad/Modules/modules.h"

#include "MinHook/include/MinHook.h"
#pragma comment(lib, "minhook.x64.lib")
#include "Hooks/Hooks.h"

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
		inline bool auto_aim = true;

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

	// called when dll has injected
	DWORD WINAPI init();

	// the main loop when init was succesfull
	void update();

	// called when wanting to uninject and cleans up
	void clean_up(int exitcode = 0);
}