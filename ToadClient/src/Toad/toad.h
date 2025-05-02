#pragma once

#include "toad_defs.h"

// JDK
#include "jni.h"
#include "jvmti.h"

#include "Toad/types.h"
#include "Toad/logger.h"
#include "Toad/config.h"

#include "Toad/Utils/helpers.h"

#include "Toad/MC/mcutils.h"
#include "Toad/MC/mappings.h"
#include "Toad/MC/active_render_info.h"
#include "Toad/MC/entity.h"
#include "Toad/MC/minecraft.h"

#include "Toad/math.h"
#include "Toad/timer.h"
#include "Toad/module.h"

#include "Modules/vars_updater.h"
#include "Modules/clicker/rand_types.h"
#include "Modules/clicker/clicker_base.h"
#include "Modules/clicker/left_autoclicker.h"
#include "Modules/clicker/right_autoclicker.h"
#include "Modules/visuals/esp.h"
#include "Modules/visuals/block_esp.h"
#include "Modules/visuals/of_screen_arrows.h"
#include "Modules/aimassist.h"
#include "Modules/no_click_delay.h"
#include "Modules/bridge_assist.h"
#include "Modules/blink.h"
#include "Modules/velocity.h"
#include "Modules/chest_stealer.h"
#include "Modules/internal_ui.h"

#include "MinHook/include/MinHook.h"
#pragma comment(lib, "minhook.x64.lib")

#include "Hooks/hook.h"
#include "Hooks/wglswapbuffers.h"
#include "Hooks/ws2_32.h"
#include "hooks/get_raw_input_data.h"

// global vars and functions 
namespace toad
{
	inline std::atomic_bool g_is_running = false;

	inline int g_screen_height = -1, g_screen_width = -1;

	inline HMODULE g_hMod;
	inline HWND g_hWnd; 

	inline JNIEnv* g_env = nullptr;
	inline JavaVM* g_jvm = nullptr;
	inline jvmtiEnv* g_jvmti_env = nullptr;

	inline Config& settings = Config::Get();

	inline Logger g_logger;

	constexpr const char* version = "1.0.0";

	/// called when dll has injected
	DWORD WINAPI ToadInit();
}