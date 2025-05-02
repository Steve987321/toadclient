#include "pch.h"
#include "toad.h"

#include "Toad/Utils/windump.h"

#include "nlohmann/json.hpp"
#include "Toad/Utils/ipc.h"

#include "Toad/MC/mapping_generator.h"

using namespace toad;

// for getting settings from loader
static std::thread Tupdate_settings;

static std::vector<std::pair<std::thread, std::string&>> cmodule_threads;

/// called when wanting to un-inject and cleans up
extern void CleanUp(int exitcode = 0, std::string_view msg = "");

/// Update the settings from the loader
///
///	Returns false when it failed to read or open the settings
extern bool UpdateSettings(Utils::IPC& ipc);

/// starts the cheat modules 
extern void InitModules();

static void PrintAllClasses()
{
	jclass* classes;
	jint cls_count;
	g_jvmti_env->GetLoadedClasses(&cls_count, &classes);

	do
	{
		// get name method id for getting class names
		jclass klass = findclass("java/lang/Class", g_env);
		if (!klass)
		{
			LOGERROR("[MappingGenerator] Can't find class Class");
			break;
		}
		jmethodID get_klass_name = g_env->GetMethodID(klass, "getName", "()Ljava/lang/String;");
		if (!get_klass_name)
		{
			LOGERROR("[MappingGenerator] Can't find String getName()");
			g_env->DeleteLocalRef(klass);
			break;
		}

		g_env->DeleteLocalRef(klass);
		for (jint i = 0; i < cls_count; i++)
		{
			jclass cls = classes[i];

			if (!cls)
				continue;

			jstring jstr = (jstring)g_env->CallObjectMethod(cls, get_klass_name);
			if (!jstr)
				continue;

			LOGDEBUG("{}", jstring2string(jstr, g_env));

			g_env->DeleteLocalRef(jstr);
		}

		g_jvmti_env->Deallocate((uint8_t*)classes);
	} while (false);

}

DWORD WINAPI toad::ToadInit()
{
#ifdef ENABLE_LOGGING
	SetUnhandledExceptionFilter(Utils::UnhandledExceptionHandler);
#endif
	LOGDEBUG("[init] Start");

	Utils::IPC ipc;

	if (!ipc.OpenMappedFile("ToadClientMappingObj"))
	{
		LOGERROR("[init] Failed to init IPC");
		CleanUp();
		return 1;
	}

	settings.g_is_ui_internal = false;
	CInternalUI::ShouldClose = true;

	if (!UpdateSettings(ipc))
	{
		CleanUp(1, "Failed to open settings");
		return 1;
	}

	// get functions from jvm.dll
	auto jvm_handle = GetModuleHandleA("jvm.dll");

	if (jvm_handle == nullptr)
	{
		CleanUp(1, "Failed to get handle from jvm.dll");
		return 1;
	}

	jvmfunc::oJNI_GetCreatedJavaVMs = reinterpret_cast<jvmfunc::hJNI_GetCreatedJavaVMs>(GetProcAddress(jvm_handle, "JNI_GetCreatedJavaVMs"));
	jvmfunc::oJVM_GetMethodIxNameUTF = reinterpret_cast<jvmfunc::hJVM_GetMethodIxNameUTF>(GetProcAddress(jvm_handle, "JVM_GetMethodIxNameUTF"));
	jvmfunc::oJVM_GetMethodIxSignatureUTF = reinterpret_cast<jvmfunc::hJVM_GetMethodIxSignatureUTF>(GetProcAddress(jvm_handle, "JVM_GetMethodIxSignatureUTF"));
	jvmfunc::oJVM_GetClassMethodsCount = reinterpret_cast<jvmfunc::hJVM_GetClassMethodsCount>(GetProcAddress(jvm_handle, "JVM_GetClassMethodsCount"));
	jvmfunc::oJVM_GetClassFieldsCount = reinterpret_cast<jvmfunc::hJVM_GetClassFieldsCount>(GetProcAddress(jvm_handle, "JVM_GetClassFieldsCount"));
	jvmfunc::oJVM_GetClassDeclaredFields = reinterpret_cast<jvmfunc::hJVM_GetClassDeclaredFields>(GetProcAddress(jvm_handle, "JVM_GetClassDeclaredFields"));
	jvmfunc::oJVM_GetArrayElement = reinterpret_cast<jvmfunc::hJVM_GetArrayElement>(GetProcAddress(jvm_handle, "JVM_GetArrayElement"));
	jvmfunc::oJVM_GetArrayLength = reinterpret_cast<jvmfunc::hJVM_GetArrayLength>(GetProcAddress(jvm_handle, "JVM_GetArrayLength"));
	jvmfunc::oJVM_GetMethodIxArgsSize = reinterpret_cast<jvmfunc::hJVM_GetMethodIxArgsSize>(GetProcAddress(jvm_handle, "JVM_GetMethodIxArgsSize"));

	jvmfunc::oJNI_GetCreatedJavaVMs(&g_jvm, 1, nullptr);

	LOGDEBUG("[init] jvm: {}", (void*)g_jvm);

	if (!g_jvm)
	{
		CleanUp(2, "jvm is null");
		return 1;
	}

	LOGDEBUG("[init] Attach current thread");

	if (g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&g_env), nullptr) != JNI_OK)
	{
		CleanUp(5, "Failed to attach current thread");
		return 1;
	}

	if (g_jvm->GetEnv((void**)&g_jvmti_env, JVMTI_VERSION_1) == JNI_OK)
	{
		LOGDEBUG("[init] jvmti ok");

		// add jvmti capabilities
		jvmtiCapabilities capabilities{};
		capabilities.can_get_bytecodes = 1;

		jvmtiError res = g_jvmti_env->AddCapabilities(&capabilities);
		if (res != jvmtiError::JVMTI_ERROR_NONE)
		{
			LOGERROR("[init] AddCapabilities returned: {}", (int)res);
		}
	}

	if (!g_env)
	{
		CleanUp(2);
		return 0;
	}

	//g_jvmti_env->GetClassLoader();

	REGISTER_HOOK(HSwapBuffers);
	REGISTER_HOOK(HWSASend);
	REGISTER_HOOK(HWSARecv);

	LOGDEBUG("[init] Initializing hooks");
	Hook::InitializeAllHooks();

	LOGDEBUG("[init] Enabling hooks");
	Hook::EnableAllHooks();

	LOGDEBUG("[init] Client type {}", static_cast<int>(settings.g_curr_client));

	//std::filesystem::path generated_mappings_file = GetDocumentsFolder();
	//generated_mappings_file /= "mapping_gen_out.txt";
	//if (std::filesystem::exists(generated_mappings_file))
	//	MappingGenerator::InitMappings(g_env, g_jvmti_env, generated_mappings_file);

	auto mcclass = Minecraft::getMcClass(g_env);
	if (mcclass == nullptr)
	{
		CleanUp(4);
		return 0;
	}

	auto eclasstemp = findclass("net.minecraft.entity.Entity", g_env);
	if (eclasstemp == nullptr)
	{
		CleanUp(6);
		return 0;
	}

	LOGDEBUG("[init] Mappings");
	mappings::init_map(g_env, mcclass, eclasstemp, settings.g_curr_client);

	g_env->DeleteLocalRef(eclasstemp);
	g_env->DeleteLocalRef(mcclass);

	//MappingGenerator::Generate(g_env, g_jvmti_env);

	g_is_running = true;

	LOGDEBUG("[init] Starting modules");
	InitModules();

	LOGDEBUG("[init] Entering main loop");
	while (g_is_running)
	{
		if (GetAsyncKeyState(VK_END)) 
			break;

		UpdateSettings(ipc);

		SLEEP(100);
	}

	CleanUp(0);
	
	return 0;
}

void CleanUp(int exitcode, std::string_view msg)
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
		if (g_jvm)
		{
			g_jvm->DetachCurrentThread();
		}

		if (g_jvmti_env)
		{
			g_jvmti_env->DisposeEnvironment();
		}

		LOGDEBUG("threads");
		CVarsUpdater::IsVerifiedCV.notify_all();
		for (auto& m : CModule::ModuleInstances)
			m->EnabledCV.notify_all();

		if (Tupdate_settings.joinable()) 
			Tupdate_settings.join();

		for (auto& [module_thread, name] : cmodule_threads)
		{
			LOGDEBUG("{}", name);
			if (module_thread.joinable())
			{
				module_thread.join();
			}
		}
			
		g_env = nullptr;
		g_jvm = nullptr;
		g_jvmti_env = nullptr;

#ifdef ENABLE_LOGGING
		LOGDEBUG("console");
		g_logger.DisposeLogger();
#endif

		FreeLibraryAndExitThread(g_hMod, 0);
	});
}

bool UpdateSettings(Utils::IPC& ipc)
{
	using namespace toad;

	using json = nlohmann::json;

	void* pMem = ipc.MapData();
	if (!pMem)
	{
		LOGWARN("Failed to map view of mapping file");
		return false;
	}

	std::string s = (char*)pMem;
	// parse buf as json and read them and set them and 

	size_t endof = s.find("END");
	if (endof == std::string::npos)
	{
		LOGERROR("Failed to find END, {}", endof);
		ipc.UnMapData();
		return false;
	}
	std::string settings_str = s.substr(0, endof);

	json data;
	try
	{
		data = json::parse(settings_str);
	}
	catch (json::parse_error& e)
	{
		LOGERROR("Json parse error: {} at: {}", e.what(), e.byte);
	}

	if (data.contains("close"))
	{
		LOGDEBUG("close flag receieved");
		g_is_running = false;
	}

	// flag that will make sure the menu will show when switching to internal ui
	static bool open_menu_once_flag = true;

	// things that are written to data till memcpy gets called will be read by the loader 

	if (data.contains("ui_internal_should_close"))
	{
		if (data["ui_internal_should_close"])
		{
			if (CInternalUI::ShouldClose && !data["ui_internal"])
			{
				open_menu_once_flag = true;
				CInternalUI::ShouldClose = false;
			}
		}
	}

	data["ui_internal_should_close"] = CInternalUI::ShouldClose;

	// rand
	if (data.contains("updatelcrand"))
	{
		std::vector<Boost> updated_boosts = {};
		std::vector<Inconsistency> updated_incs = {};
		std::vector<Inconsistency> updated_incs2 = {};

		json boosts = data["lc_randb"];
		json inc = data["lc_randi"];
		json inc2 = data["lc_randi2"];

		for (const auto& element : boosts.items())
		{
			auto boostId = std::stoi(element.key());

			auto n = element.value().at("n");
			auto dur = element.value().at("dur");
			auto tdur = element.value().at("tdur");
			auto fqmin = element.value().at("fqmin");
			auto fqmax = element.value().at("fqmax");

			updated_boosts.emplace_back(n, dur, tdur, fqmin, fqmax, boostId);
		}
		for (const auto& element : inc.items())
		{
			auto nmin = element.value().at("nmin");
			auto nmax = element.value().at("nmax");
			auto c = element.value().at("c");
			auto f = element.value().at("f");

			updated_incs.emplace_back(nmin, nmax, c, f);
		}
		for (const auto& element : inc2.items())
		{
			auto nmin = element.value().at("nmin");
			auto nmax = element.value().at("nmax");
			auto c = element.value().at("c");
			auto f = element.value().at("f");

			updated_incs2.emplace_back(nmin, nmax, c, f);
		}

		auto& rand = CLeftAutoClicker::GetRand();

		rand.boosts = updated_boosts;
		rand.inconsistencies = updated_incs;
		rand.inconsistencies2 = updated_incs2;

		data["done"] = 0;
	}

	if (data.contains("esp_font"))
	{
		settings.esp_font_path = data["esp_font"];
		HSwapBuffers::UpdateFont();
		data["done"] = 0;
	}

	if (data.contains("path"))
	{
		settings.loader_path = data["path"];
		data["donepath"] = 0;
	}

	std::stringstream ss;
	ss << data << "END";
	memcpy(pMem, ss.str().c_str(), ss.str().length()); 

	if (CInternalUI::ShouldClose)
	{
		settings.g_is_ui_internal = false;
		CInternalUI::MenuIsOpen = false;
	}
	else
	{
		settings.g_is_ui_internal = data["ui_internal"];
		CInternalUI::ShouldClose = false;

		if (open_menu_once_flag && settings.g_is_ui_internal)
		{
			CInternalUI::MenuIsOpen = true;
			open_menu_once_flag = false;
		}
	}

	if (settings.g_is_ui_internal)
	{
		ipc.UnMapData();
		SLEEP(1000);
		return false;
	}

	settings.g_curr_client = data["client_type"];
	settings.loaded_config = data["config"];

	std::string error_msg;

	// For clicker update delays 
	float lmin_cps_old = settings.lc_min_cps;
	float lmax_cps_old = settings.lc_max_cps;
	float rcps_old = settings.rc_cps;

	if (!settings.LoadSettings(data.dump(), error_msg))
	{
		ipc.UnMapData();
		LOGERROR("Error loading settings: {}", error_msg);
		return false;
	}

	CModule::UpdateModuleEnableStates();

	if (settings.lc_min_cps != lmin_cps_old || settings.lc_max_cps != lmax_cps_old)
		CLeftAutoClicker::SetDelays(settings.lc_min_cps, settings.lc_max_cps);

	if (settings.rc_cps != rcps_old)
		CRightAutoClicker::SetDelays(settings.rc_cps);

	ipc.UnMapData();

	return true;
}

// wow 
static bool modules_always_on_flag = true;

void InitModules()
{
	REGISTER_CMODULE(CVarsUpdater, "Vars updater", modules_always_on_flag);
	REGISTER_CMODULE(CLeftAutoClicker, "Left autoclicker", settings.lc_enabled);
	REGISTER_CMODULE(CRightAutoClicker, "Right autoclicker", settings.rc_enabled);
	REGISTER_CMODULE(CAimAssist, "Aim assist", settings.aa_enabled);
	REGISTER_CMODULE(CEsp, "Esp", settings.esp_enabled);
	REGISTER_CMODULE(CBlockEsp, "Block esp", settings.besp_enabled);
	REGISTER_CMODULE(CVelocity, "Velocity", settings.vel_enabled);
	REGISTER_CMODULE(CBlink, "Blink", settings.bl_enabled);
	REGISTER_CMODULE(CInternalUI, "Internal ui", modules_always_on_flag, false, true);
	REGISTER_CMODULE(CBridgeAssist, "Bridge assist", settings.ba_enabled);
	REGISTER_CMODULE(CNoClickDelay, "Auto bridge", settings.ncd_enabled);
	REGISTER_CMODULE(CChestStealer, "Chest Stealer", settings.cs_enabled);

	cmodule_threads.reserve(CModule::ModuleInstances.size());

	for (CModule* Module : CModule::ModuleInstances)
	{
		LOGDEBUG("{}", Module->Name);

		if (Module->IsOnlyRendering)
		{
			Module->Initialized = true;
			continue;
		}

		//LOGDEBUG("Starting cheat module: {}", Module->Name);

		cmodule_threads.emplace_back([Module]
			{
				JNIEnv* env = nullptr;

				g_jvm->GetEnv(reinterpret_cast<void**>(&env), g_env->GetVersion());
				g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr);

				Minecraft mc;
				mc.env = env;

				Module->SetEnv(env);
				Module->SetMC(mc);

				Module->Initialized = true;

				while (g_is_running)
				{
					Module->PreUpdate();
					Module->Update(CVarsUpdater::theLocalPlayer);
				}

				LOGDEBUG("Closing cheat module: {}", Module->Name);

				mc.Clean();

				g_jvm->DetachCurrentThread();
			}, 
			Module->Name);
	}

	for (const CModule* m : CModule::ModuleInstances)
	{
		while (!m->Initialized)
		{
			LOGDEBUG("{}", m->Name);
			Sleep(1);
		}
	}
}
