#pragma once

#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

// use this when precision isn't required but the CPU should be saved
#define SLOW_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

// macro that inherits from essential classes for a cheat module
#define SET_MODULE_CLASS(T) final : public toadll::c_Singleton<T>, public CModule

#include "Toad/Types.h"
#include "Toad/Logger.h"

#include "helpers.h"

#include "Toad/MC/Utils/mcutils.h"
#include "Toad/MC/mappings.h"
#include "Toad/MC/ActiveRenderInfo/ActiveRenderInfo.h"
#include "Toad/MC/Entity/Entity.h"
#include "Toad/MC/Minecraft.h"

#include "timer.h"
#include "singleton.h"
#include "module.h"

#include "Modules/esp.h"
#include "Modules/aimassist.h"
#include "Modules/velocity.h"

#include "Toad/Modules/modules.h"

#include "MinHook/include/MinHook.h"
#pragma comment(lib, "minhook.x64.lib")

#include "Hooks/Hooks.h"
#include "Hooks/wglswapbuffers.h"
#include "Hooks/ws2_32.h"

// global vars and functions 
namespace toadll
{
	inline minecraft_client curr_client = minecraft_client::Lunar;

	inline std::atomic_bool is_running = false;

	inline std::thread Tupdate_settings;
	inline std::thread Tupdate_cursorinfo;
	inline std::thread Tupdate_hookvars;

	inline bool is_cursor_shown = false;

	inline int screen_height = -1, screen_width = -1;
	inline GLfloat modelview[16];
	inline GLfloat projection[16];

	inline HMODULE hMod;

	inline float partialTick = 0;
	inline float renderPartialTick = 0;

	inline JNIEnv* env = nullptr;
	inline JavaVM* jvm = nullptr;

	// settings
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

	// called when dll has injected
	DWORD WINAPI init();

	// the main loop when init was succesfull
	void update();

	// called when wanting to uninject and cleans up
	void clean_up(int exitcode = 0);
}