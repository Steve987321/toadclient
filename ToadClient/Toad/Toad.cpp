 #include "pch.h"
#include "Toad.h"

#include "nlohmann/json.hpp"

using namespace toadll;

// for getting settings from loader
constexpr static size_t bufsize = 1000;
std::thread Tupdate_settings;

inline std::vector<std::thread> cmodule_threads;

/// called when wanting to un-inject and cleans up
extern void clean_up(int exitcode = 0, std::string_view msg = "");

/// Update the settings from the loader
///
///	@return False when it failed to read or open the settings
extern bool UpdateSettings();

/// starts the cheat modules 
extern void init_modules();

DWORD WINAPI toadll::init()
{
	if (!UpdateSettings())
	{
		clean_up(1, "Failed to open settings");
		return 1;
	}

#ifdef ENABLE_LOGGING
	Logger::GetInstance();
	SetConsoleCtrlHandler(NULL, true);
#endif

	// get functions from jvm.dll
	auto jvmHandle = GetModuleHandleA("jvm.dll");

	if (jvmHandle == nullptr)
	{
		clean_up(1, "Failed to get handle from jvm.dll");
		return 1;
	}

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
	{
		clean_up(5, "failed to attach current thread");
		return 1;
	}

	/*jvmtiEnv* jvmtiEnv = nullptr;
	if (g_jvm->GetEnv(reinterpret_cast<void**>(&jvmtiEnv), JVMTI_VERSION_1) == JNI_OK)
	{
		log_Debug("yoooo");
		jint n;
		jclass* classes;
		jvmtiEnv->GetLoadedClasses(&n, &classes);
		
		for (int i = 0; i < n; i++)
		{
			auto klass = classes[i];
			auto cls = findclass("java/lang/Class", g_env);
			if (!cls)
				continue;

			auto mId = g_env->GetMethodID(cls, "getName", "()Ljava/lang/String;");
			if (!mId)
			{
				g_env->DeleteLocalRef(cls);
				continue;
			}
			g_env->DeleteLocalRef(cls);

			const auto jstrname = static_cast<jstring>(g_env->CallObjectMethod(klass, mId));
			if (!jstrname)
			{
				log_Error("jstrname = nit shjicyed");
				continue;
			}
			log_Debug(jstring2string(jstrname, g_env).c_str());
		}
	}
	else
	{
		log_Debug("NOOO");
	}*/
	if (!g_env)
	{
		clean_up(2);
		return 0;
	}

	// Instantiate hooks 
	HSwapBuffers::GetInstance();
	HWSASend::GetInstance();
	HWSARecv::GetInstance();

	LOGDEBUG("Initialize hooks");
	Hook::InitializeAllHooks();

	LOGDEBUG("Enabling hooks");
	Hook::EnableAllHooks();

	if (toad::g_curr_client == toad::MC_CLIENT::NOT_UPDATED)
	{
		clean_up(44);
		return 0;
	}
	LOGDEBUG("client type {}", static_cast<int>(toad::g_curr_client));

	auto mcclass = Minecraft::getMcClass(g_env);
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

	LOGDEBUG("starting modules");
	init_modules();

	g_is_running = true;

	LOGDEBUG("entering main loop");
	while (g_is_running)
	{
		if (GetAsyncKeyState(VK_END)) break;

		UpdateSettings();

		SLEEP(100);
	}
	clean_up(0);
	
	return 0;
}


void clean_up(int exitcode, std::string_view msg)
{
	static std::once_flag flag;

	std::call_once(flag, [&]
		{
			g_is_running = false;

	if (!msg.empty())
		LOGDEBUG("closing: {}, {}", exitcode, msg.data());
	else
		LOGDEBUG("closing: {}", exitcode);

	LOGDEBUG("hooks");
	Hook::CleanAllHooks();

	LOGDEBUG("jvm");
	if (g_jvm != nullptr)
	{
		g_jvm->DetachCurrentThread();
	}

	LOGDEBUG("threads");
	if (Tupdate_settings.joinable()) Tupdate_settings.join();

	for (auto& ModuleThread : cmodule_threads)
		if (ModuleThread.joinable()) ModuleThread.join();

	g_env = nullptr;
	g_jvm = nullptr;

#ifdef ENABLE_LOGGING
	LOGDEBUG("console");
	Logger::GetInstance()->DisposeLogger();
#endif

	FreeLibraryAndExitThread(g_hMod, 0);

		});
}

bool UpdateSettings()
{
	using namespace toad;

	using json = nlohmann::json;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"ToadClientMappingObj");
	if (hMapFile == NULL)
	{
		g_is_running = false;
		return false;
	}

	const auto pMem = MapViewOfFile(hMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, bufsize);
	if (pMem == NULL)
	{
		CloseHandle(hMapFile);
		return false;
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
		SLEEP(1000);
		return false;
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

	return true;
}

void init_modules()
{
	// Instantiate all cheat modules
	CVarsUpdater::GetInstance();
	CLeftAutoClicker::GetInstance();
	CRightAutoClicker::GetInstance();
	CAimAssist::GetInstance();
	CEsp::GetInstance();
	CBlockEsp::GetInstance();
	CVelocity::GetInstance();
	CBlink::GetInstance();
	CInternalUI::GetInstance();

	//CAutoPot::get_instance();

	for (const auto& Module : CModule::moduleInstances)
	{
		cmodule_threads.emplace_back([&]()
			{
				JNIEnv* env = nullptr;

		g_jvm->GetEnv(reinterpret_cast<void**>(&env), g_env->GetVersion());
		g_jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env), nullptr);

		auto mcclass = Minecraft::getMcClass(env);
		auto mc = std::make_unique<Minecraft>();

		mc->env = env;
		mc->mcclass = mcclass;

		Module->SetEnv(env);
		Module->SetMC(mc);

		Module->Initialized = true;

		while (g_is_running)
		{
			Module->PreUpdate();

			if (!CVarsUpdater::IsVerified)
			{
				SLEEP(100);
			}
			else
			{
				const auto& lPlayer = CVarsUpdater::theLocalPlayer;
				Module->Update(lPlayer);
			}
		}

		g_jvm->DetachCurrentThread();
			});
	}

	// wait for all modules to be initalized before updating 
	std::ranges::borrowed_iterator_t<std::vector<CModule*>&> it;
	do
	{
		it = std::ranges::find_if(CModule::moduleInstances, [](const auto& mod) { return !mod->Initialized; });

	} while (it != CModule::moduleInstances.end());

}