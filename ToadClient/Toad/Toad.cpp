 #include "pch.h"
#include "Toad.h"

#include "nlohmann/json.hpp"

//#include <glew-2.1.0/include/GL/glew.h>
//#include <gl/GL.h>

// for getting settings
constexpr static size_t bufsize = 1000;

int stage = 0;

std::thread Tupdate_settings;
std::thread Tupdate_cursorinfo;

namespace toadll
{
	void Fupdate_settings()
	{
		HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"ToadClientMappingObj");
		if (hMapFile == NULL)
		{
			g_is_running = false;
			log_Debug("exit 11");
			return;
		}
		const auto buf = (LPCSTR)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, bufsize);
		if (buf == NULL)
		{
			log_Debug("buf = 0");
			//clean_up(12);
			CloseHandle(hMapFile);
			return;
		}
		std::string s = buf;
		//log_Debug(s.c_str());
		// parse buf as json and read them and set them and 

		auto endof = s.find("END");
		std::string settings = s.substr(0, endof);
		//log_Debug(settings.c_str());

		using json = nlohmann::json;
		json data = json::parse(settings);

		using namespace toad;

		// left auto clicker
		left_clicker::enabled = data["lc_enabled"];
		left_clicker::cps = data["lc_cps"];
		left_clicker::break_blocks = data["lc_breakblocks"];
		left_clicker::block_hit = data["lc_blockhit"];
		left_clicker::block_hit_ms = data["lc_blockhitms"];
		left_clicker::targeting_affects_cps = data["lc_smartcps"];
		left_clicker::weapons_only = data["lc_weaponsonly"];
		left_clicker::trade_assist = data["lc_tradeassist"];

		// right auto clicker
		right_clicker::enabled = data["rc_enabled"];
		right_clicker::blocks_only = data["rc_blocks_only"];
		right_clicker::start_delayms = data["rc_start_delay"];

		// aim assist
		aa::enabled = data["aa_enabled"];
		aa::distance = data["aa_distance"];
		aa::speed = data["aa_speed"];
		aa::horizontal_only = data["aa_horizontal_only"];
		aa::fov = data["aa_fov"];
		aa::invisibles = data["aa_invisibles"];
		aa::targetFOV = data["aa_targetFOV"];
		aa::always_aim = data["aa_always_aim"];
		aa::aim_at_closest_point = data["aa_multipoint"];
		aa::lock_aim = data["aa_lockaim"];

		// auto bridge
		auto_bridge::enabled = data["ab_enabled"];
		auto_bridge::pitch_check = data["ab_pitch_check"];

		// velocity
		velocity::enabled = data["vel_enabled"];
		velocity::jump_reset = data["vel_jumpreset"];
		velocity::horizontal = data["vel_horizontal"];
		velocity::vertical = data["vel_vertical"];
		velocity::chance = data["vel_chance"];
		velocity::delay = data["vel_delay"];

		// esp
		esp::enabled = data["esp_enabled"];
		esp::lineCol[0] = data["esp_linecolr"];
		esp::lineCol[1] = data["esp_linecolg"];
		esp::lineCol[2] = data["esp_linecolb"];
		esp::lineCol[3] = data["esp_linecola"];
		esp::fillCol[0] = data["esp_fillcolr"];
		esp::fillCol[1] = data["esp_fillcolg"];
		esp::fillCol[2] = data["esp_fillcolb"];
		esp::fillCol[3] = data["esp_fillcola"];

		CLeftAutoClicker::SetDelays(left_clicker::cps);
		CRightAutoClicker::SetDelays(right_clicker::cps);

		UnmapViewOfFile(buf);
		CloseHandle(hMapFile);
	}

	DWORD WINAPI init()
	{
#ifdef ENABLE_LOGGING
		p_Log = std::make_unique<c_Logger>();
		SetConsoleCtrlHandler(NULL, true);
#endif

		GetCurrWindowHWND(&g_hWnd);
		if (!g_hWnd)
			return 1;

		// get functions from jvm.dll
		auto jvmHandle = GetModuleHandleA("jvm.dll");

		if (jvmHandle == nullptr)
			return 1;

		jvmfunc::oJNI_GetCreatedJavaVMs = reinterpret_cast<jvmfunc::hJNI_GetCreatedJavaVMs>(GetProcAddress(jvmHandle, "JNI_GetCreatedJavaVMs"));
		jvmfunc::oJVM_GetMethodIxNameUTF = reinterpret_cast<jvmfunc::hJVM_GetMethodIxNameUTF>(GetProcAddress(jvmHandle, "JVM_GetMethodIxNameUTF"));
		jvmfunc::oJVM_GetMethodIxSignatureUTF = reinterpret_cast<jvmfunc::hJVM_GetMethodIxSignatureUTF>(GetProcAddress(jvmHandle, "JVM_GetMethodIxSignatureUTF"));
		jvmfunc::oJVM_GetClassMethodsCount = reinterpret_cast<jvmfunc::hJVM_GetClassMethodsCount>(GetProcAddress(jvmHandle, "JVM_GetClassMethodsCount"));
		jvmfunc::oJVM_GetClassFieldsCount = reinterpret_cast<jvmfunc::hJVM_GetClassFieldsCount>(GetProcAddress(jvmHandle, "JVM_GetClassFieldsCount"));
		jvmfunc::oJVM_GetClassDeclaredFields = reinterpret_cast<jvmfunc::hJVM_GetClassDeclaredFields>(GetProcAddress(jvmHandle, "JVM_GetClassDeclaredFields"));
		jvmfunc::oJVM_GetArrayElement = reinterpret_cast<jvmfunc::hJVM_GetArrayElement>(GetProcAddress(jvmHandle, "JVM_GetArrayElement"));
		jvmfunc::oJVM_GetArrayLength = reinterpret_cast<jvmfunc::hJVM_GetArrayLength>(GetProcAddress(jvmHandle, "JVM_GetArrayLength"));
		jvmfunc::oJVM_GetMethodIxArgsSize = reinterpret_cast<jvmfunc::hJVM_GetMethodIxArgsSize>(GetProcAddress(jvmHandle, "JVM_GetMethodIxArgsSize"));

		jvmfunc::oJNI_GetCreatedJavaVMs(&g_jvm, 1, nullptr);

		if (g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&g_env), nullptr) != JNI_OK)
			return 1;

		if (!g_env)
		{
			clean_up(2);
			return 0;
		}

		auto mcclass = c_Minecraft::get_mcclass(g_env);
		if (mcclass == nullptr)
		{
			clean_up(4);
			return 0;
		}

		if (!c_Swapbuffershook::get_instance()->init())
		{
			log_Error("failed to hook swapbuffers");
			clean_up(3);
			return 0;
		}

		if (!c_WSASend::get_instance()->init())
		{
			log_Error("failed to hook WSA");
			clean_up(22);
			return 0;
		}

		auto eclasstemp = findclass("net.minecraft.entity.Entity", g_env);
		if (eclasstemp == nullptr)
		{
			clean_up(6);
			return 0;
		}

		mappings::init_map(g_env, mcclass, eclasstemp, curr_client);

		g_env->DeleteLocalRef(eclasstemp);
		g_env->DeleteLocalRef(mcclass);

		g_is_running = true;

		log_Debug("enabling hooks");

		c_Swapbuffershook::get_instance()->enable();
		c_WSASend::get_instance()->enable();

		log_Debug("Starting modules");

		modules::initialize();

		log_Debug("Initialized modules");

		// main loop
		while (g_is_running)
		{
			if (GetAsyncKeyState(VK_END)) break;

			Fupdate_settings();

			SLOW_SLEEP(100);

		}
		clean_up(0);
		return 0;
	}

	void clean_up(int exitcode)
	{
		static std::once_flag flag;

		std::call_once(flag, [&]
			{
			g_is_running = false;
			log_Debug("closing: %d", exitcode);
			if (!c_Swapbuffershook::get_instance()->is_null())
				c_Swapbuffershook::get_instance()->dispose();

			if (!c_WSASend::get_instance()->is_null())
				c_WSASend::get_instance()->dispose();

			log_Debug("jvm");
			if (g_jvm != nullptr)
			{
				g_jvm->DetachCurrentThread();
			}

			log_Debug("threads");
			if (Tupdate_settings.joinable()) Tupdate_settings.join();
			if (Tupdate_cursorinfo.joinable()) Tupdate_cursorinfo.join();

			for (auto& ModuleThread : modules::threads)
				if (ModuleThread.joinable()) ModuleThread.join();

			g_env = nullptr;
			g_jvm = nullptr;

#ifdef ENABLE_LOGGING
			log_Debug("console");
			p_Log->dispose_console();
#endif
			FreeLibraryAndExitThread(g_hMod, 0);
			});
	}
}
