#pragma once

#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

// use this when precision isn't required but the CPU should be saved
#define SLOW_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

// macro that inherits from essential classes for a cheat module
#define SET_MODULE_CLASS(T) final : public toadll::c_Singleton<T>, public CModule

// Settings/config for modules are stored in the Loader folder
#include "../../Loader/src/global_settings.h"

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

#include "Modules/autoclicker.h"
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

	inline std::atomic_bool g_is_running = false;

	inline std::thread Tupdate_settings;
	inline std::thread Tupdate_cursorinfo;
	inline std::thread Tupdate_hookvars;

	inline bool g_is_cursor_shown = false;

	inline int screen_height = -1, screen_width = -1;
	inline GLfloat modelview[16];
	inline GLfloat projection[16];

	inline HMODULE g_hMod;
	inline HWND g_hWnd; 

	inline float g_partialTick = 0;
	inline float g_renderPartialTick = 0;

	inline JNIEnv* g_env = nullptr;
	inline JavaVM* g_jvm = nullptr;

	// called when dll has injected
	DWORD WINAPI init();

	// the main loop when init was succesfull
	void update();

	// called when wanting to uninject and cleans up
	void clean_up(int exitcode = 0);
}