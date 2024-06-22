#pragma once

#define ENABLE_LOGGING

#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

// used when precision isn't required but the CPU should be saved
#define SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

// inherits from essential classes for a cheat module
#define SET_MODULE_CLASS(T) final : public toadll::Singleton<T>, public toadll::CModule

// inherits from essential classes for a hook
#define SET_HOOK_CLASS(T) final : public toadll::Singleton<T>, public toadll::Hook

// Settings/config variables for modules are stored in the Loader project
#include "../../Loader/src/global_settings.h"

#include "Toad/Types.h"
#include "Toad/Logger/logger.h"

#include "helpers.h"

#include "Toad/MC/mcutils.h"
#include "Toad/MC/mappings.h"
#include "Toad/MC/active_render_info.h"
#include "Toad/MC/entity.h"
#include "Toad/MC/minecraft.h"

#include "math.h"
#include "timer.h"
#include "singleton.h"
#include "module.h"

#include "Modules/vars_updater.h"
#include "Modules/clicker/rand_types.h"
#include "Modules/clicker/clicker_base.h"
#include "Modules/clicker/left_autoclicker.h"
#include "Modules/clicker/right_autoclicker.h"
#include "Modules/visuals/esp.h"
#include "Modules/visuals/block_esp.h"
#include "Modules/visuals/of_screen_arrows.h"
#include "Modules/autopot.h"
#include "Modules/aimassist.h"
#include "Modules/no_click_delay.h"
#include "Modules/bridge_assist.h"
#include "Modules/blink.h"
#include "Modules/reach.h"
#include "Modules/velocity.h"
#include "Modules/internal_ui.h"

#include "MinHook/include/MinHook.h"
#pragma comment(lib, "minhook.x64.lib")

#include "Hooks/Hooks.h"
#include "Hooks/wglswapbuffers.h"
#include "Hooks/ws2_32.h"

// global vars and functions 
namespace toadll
{
	inline std::atomic_bool g_is_running = false;

	inline int g_screen_height = -1, g_screen_width = -1;

	inline HMODULE g_hMod;
	inline HWND g_hWnd; 

	inline JNIEnv* g_env = nullptr;
	inline JavaVM* g_jvm = nullptr;
	inline jvmtiEnv* g_jvmti_env = nullptr;

	/// called when dll has injected
	DWORD WINAPI init();
}