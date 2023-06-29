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
		using namespace toad;

		using json = nlohmann::json;

		HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"ToadClientMappingObj");
		if (hMapFile == NULL)
		{
			g_is_running = false;
			log_Debug("exit 11");
			return;
		}

		const auto pMem = MapViewOfFile(hMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, bufsize);
		if (pMem == NULL)
		{
			log_Debug("buf = 0");
			CloseHandle(hMapFile);
			return;
		}

		std::string s = (LPCSTR)pMem;
		//log_Debug(s.c_str());
		// parse buf as json and read them and set them and 

		auto endof = s.find("END");
		std::string settings = s.substr(0, endof);
		//log_Debug(settings.c_str());

		json data = json::parse(settings);

		// flag that will make sure the menu will show when switching to internal ui
		static bool openMenuOnceFlag = true; 
		if (data.contains("ui_internal_should_close"))
		{
			if (data["ui_internal_should_close"])
			{
				if (CInternalUI::ShouldClose && !data["ui_internal"])
				{
					openMenuOnceFlag = true;
					CInternalUI::ShouldClose = false;
				}
			}
		}

		data["ui_internal_should_close"] = CInternalUI::ShouldClose;

		std::stringstream ss;
		ss << data << "END";
		memcpy(pMem, ss.str().c_str(), ss.str().length());

		if (CInternalUI::ShouldClose)
		{
			g_is_ui_internal = false;
			CInternalUI::MenuIsOpen = false;
		}
		else
		{
			g_is_ui_internal = data["ui_internal"];
			CInternalUI::ShouldClose = false;

			if (openMenuOnceFlag && g_is_ui_internal)
			{
				CInternalUI::MenuIsOpen = true;
				openMenuOnceFlag = false;
			}
		}

		if (g_is_ui_internal)
		{
			UnmapViewOfFile(pMem);
			CloseHandle(hMapFile);
			SLOW_SLEEP(1000);
			return;
		}

		g_curr_client = data["client_type"];

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
		right_clicker::cps = data["rc_cps"];
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

		// blink
		blink::enabled = data["bl_enabled"];
		blink::key = data["bl_key"];
		blink::disable_on_hit = data["bl_stop_on_hit"];
		blink::stop_rec_packets = data["bl_stop_incoming_packets"];
		blink::show_trail = data["bl_show_trail"];
		blink::limit_seconds = data["bl_limit_seconds"];

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

		// block esp
		block_esp::enabled = data["blockesp_enabled"];
		json blockArray = data["block_esp_array"];
		std::unordered_map<int, ImVec4> tmpList = {};
		for (const auto& element : blockArray.items())
		{
			int id = std::stoi(element.key());
			float r = element.value().at("x").get<float>();
			float g = element.value().at("y").get<float>();
			float b = element.value().at("z").get<float>();
			float a = element.value().at("w").get<float>();
			tmpList[id] = { r,g,b,a };
		}

		block_esp::block_list = tmpList;

		CLeftAutoClicker::SetDelays(left_clicker::cps);
		CRightAutoClicker::SetDelays(right_clicker::cps);

		UnmapViewOfFile(pMem);
		CloseHandle(hMapFile);
	}

	DWORD WINAPI init()
	{
#ifdef ENABLE_LOGGING
		p_Log = std::make_unique<c_Logger>();
		SetConsoleCtrlHandler(NULL, true);
#endif

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

		CSwapBuffers::get_instance();
		CWSASend::get_instance();
		c_WSARecv::get_instance();

		log_Debug("Initialize hooks");
		CHook::InitializeAllHooks();

		log_Debug("Enabling hooks");
		CHook::EnableAllHooks();

		Fupdate_settings();

		if (toad::g_curr_client == toad::minecraft_client::NOT_UPDATED)
		{
			clean_up(44);
			return 0;
		}

		log_Debug("client type %d", toad::g_curr_client);

		auto mcclass = c_Minecraft::getMcClass(g_env);
		if (mcclass == nullptr)
		{
			clean_up(4);
			return 0;
		}

		auto eclasstemp = findclass("net.minecraft.entity.Entity", g_env);
		if (eclasstemp == nullptr)
		{
			clean_up(6);
			return 0;
		}

		mappings::init_map(g_env, mcclass, eclasstemp, toad::g_curr_client);

		g_env->DeleteLocalRef(eclasstemp);
		g_env->DeleteLocalRef(mcclass);

		log_Debug("Starting modules");
		modules::initialize();

		g_is_running = true;

		log_Debug("entering main loop");
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

			log_Debug("hooks");
			CHook::CleanAllHooks();

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
