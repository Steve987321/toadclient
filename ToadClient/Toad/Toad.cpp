 #include "pch.h"
#include "Toad.h"

#include "nlohmann/json.hpp"

//#include <glew-2.1.0/include/GL/glew.h>
//#include <gl/GL.h>

constexpr static size_t bufsize = 1000;
int stage = 0;

namespace toadll
{
	inline void Fupdate_settings()
	{
		while (g_is_running)
		{
			HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"ToadClientMappingObj");
			if (hMapFile == NULL)
			{
				g_is_running = false;
				std::cout << "exit 11\n";
				break;
			}
			const auto buf = (LPCSTR)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, bufsize);
			if (buf == NULL)
			{
				std::cout << "buf = 0\n";
				//clean_up(12);
				CloseHandle(hMapFile);
				continue;
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

			// auto clicker
			clicker::enabled = data["lcenabled"];
			clicker::cps = data["lccps"];
			clicker::block_hit = data["lcblockhit"];
			clicker::block_hit_ms = data["lcblockhitms"];
			clicker::targeting_affects_cps = data["lcsmartcps"];
			clicker::weapons_only = data["lcweaponsonly"];
			clicker::trade_assist = data["lctradeassist"];

			// aim assist
			aa::enabled = data["aaenabled"];
			aa::distance = data["aadistance"];
			aa::speed = data["aaspeed"];
			aa::horizontal_only = data["aahorizontal_only"];
			aa::fov = data["aafov"];
			aa::invisibles = data["aainvisibles"];
			aa::targetFOV = data["aatargetFOV"];
			aa::always_aim = data["aaalways_aim"];
			aa::aim_at_closest_point = data["aamultipoint"];
			aa::lock_aim = data["aalockaim"];
	
			// auto bridge
			auto_bridge::enabled = data["abenabled"];
			auto_bridge::pitch_check = data["abpitch_check"];

			// velocity
			velocity::enabled = data["velenabled"];
			velocity::horizontal = data["velhorizontal"];
			velocity::vertical = data["velvertical"];
			velocity::chance = data["velchance"];
			velocity::delay = data["veldelay"];

			CLeftAutoClicker::SetDelays();

			UnmapViewOfFile(buf);
			CloseHandle(hMapFile);

			SLOW_SLEEP(100);
		}
	}

	DWORD WINAPI init()
	{
		p_Log = std::make_unique<c_Logger>();
		SetConsoleCtrlHandler(NULL, true);

		g_hWnd = GetCurrentWindowHandle();
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

		// threads
		Tupdate_settings = std::thread([] { Fupdate_settings(); });
		Tupdate_cursorinfo = std::thread([]
			{
				while (g_is_running)
				{
					CURSORINFO ci{ sizeof(CURSORINFO) };
					if (GetCursorInfo(&ci))
					{
						auto handle = reinterpret_cast<int>(ci.hCursor);
						g_is_cursor_shown = (handle > 50000 && handle < 1000000 || handle == 13961697);
					}
				}
			});

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

			log_Debug("console");
			p_Log->dispose_console();

			FreeLibraryAndExitThread(g_hMod, 0);
			});
	}
}
