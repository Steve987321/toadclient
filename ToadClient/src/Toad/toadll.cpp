#include "pch.h"
#include "toadll.h"

#include "nlohmann/json.hpp"

#include "../../Loader/src/Application/config.h"
#include "../../Loader/src/Application/config.cpp"

#include "Toad/MC/mapping_generator.h"

using namespace toadll;

// for getting settings from loader
std::thread Tupdate_settings;

inline std::vector<std::pair<std::thread, std::string&>> cmodule_threads;

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
#ifdef ENABLE_LOGGING
	Logger::GetInstance();
#endif
	LOGDEBUG("[init] Start");

	toad::g_is_ui_internal = false;
	CInternalUI::ShouldClose = true;

	if (!UpdateSettings())
	{
		clean_up(1, "Failed to open settings");
		return 1;
	}

	// get functions from jvm.dll
	auto jvm_handle = GetModuleHandleA("jvm.dll");

	if (jvm_handle == nullptr)
	{
		clean_up(1, "Failed to get handle from jvm.dll");
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
		clean_up(2, "jvm is null");
		return 1;
	}

	LOGDEBUG("[init] Attach current thread");

	if (g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&g_env), nullptr) != JNI_OK)
	{
		clean_up(5, "Failed to attach current thread");
		return 1;
	}

#ifdef ENABLE_LOGGING
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
#endif 


	//	jint n = 0;
	//	jclass* classes = nullptr;
	//	g_jvmti_env->GetLoadedClasses(&n, &classes);
	//	LOGDEBUG("classes count {}", n);

	//	auto cls = findclass("java/lang/Class", g_env);
	//	auto mId = g_env->GetMethodID(cls, "getName", "()Ljava/lang/String;");
	//	
	//	do 
	//	{
	//		if (!cls)
	//			break;

	//		if (!mId)
	//		{
	//			g_env->DeleteLocalRef(cls);
	//			break;
	//		}

	//		for (jint i = 0; i < n; i++)
	//		{
	//			auto klass = classes[i];

	//			if (!klass)
	//				continue;

	//			const auto jstrname = static_cast<jstring>(g_env->CallObjectMethod(klass, mId));
	//			if (!jstrname)
	//			{
	//				LOGERROR("jstrname = nit shjicyed");
	//				continue;
	//			}
	//					
	//			//jfieldID* fields = nullptr;
	//			//jint field_count = -1;
	//			jmethodID* methods = nullptr;
	//			jint method_count = -1;
	//			//jint modifiers = -1;
	//			//g_jvmti_env->GetClassModifiers(klass, &modifiers);
	//			g_jvmti_env->GetClassMethods(klass, &method_count, &methods);
	//			//g_jvmti_env->GetClassFields(klass, &method_count, &fields);

	//			bool f = false;
	//			if (methods)
	//			{
	//				for (jint j = 0; j < method_count; j++)
	//				{
	//					char* name = nullptr;
	//					char* signature = nullptr;
	//					char* generic = nullptr;

	//					auto method = methods[j];
	//					if (!method)
	//						continue;

	//					auto err = g_jvmti_env->GetMethodName(method, &name, &signature, &generic);
	//					if (err != JVMTI_ERROR_NONE)
	//						continue;

	//					std::string name_str;
	//					if (name)
	//						name_str = name;

	//					if (name_str == "getMinecraft")
	//					{
	//						/*jboolean is_obselete = false;
	//						jint access_flags = -1;

	//						auto err1 = g_jvmti_env->IsMethodObsolete(methods[j], &is_obselete);
	//						auto err2 = g_jvmti_env->GetMethodModifiers(methods[j], &access_flags);
	//						bool is_static = false;

	//						if (err2 == JVMTI_ERROR_NONE)
	//						{
	//							is_static = access_flags & 0x0008;
	//						}
	//						*/
	//						LOGDEBUG("Found getmc: name: {} | sig: {} | generic: {} | count {}",
	//							name_str,
	//							signature ? signature : "None",
	//							generic ? generic : "None",
	//							(int)method_count
	//						);
	//						f = true;
	//						break;
	//					}

	//					g_jvmti_env->Deallocate((unsigned char*)name);
	//					g_jvmti_env->Deallocate((unsigned char*)signature);
	//					g_jvmti_env->Deallocate((unsigned char*)generic);
	//				}
	//			}

	//			//if (auto err = g_jvmti_env->Deallocate((unsigned char*)methods); err != JVMTI_ERROR_NONE)
	//			//{
	//			//	LOGERROR("Failed to deallocate methods array: {}", (int)err);
	//			//}
	//			//if (auto err = g_jvmti_env->Deallocate((unsigned char*)fields); err != JVMTI_ERROR_NONE)
	//			//{
	//			//	LOGERROR("Failed to deallocate fields array: {}", (int)err);
	//			//}

	//			//LOGDEBUG("{} | fields: {} methods: {} modifiers: {}", jstring2string(jstrname, g_env).c_str(), field_count, method_count, modifiers);

	//			if (f)
	//				break;
	//		}

	//	} while (false);

	//	//g_jvmti_env->Deallocate((unsigned char*)classes);
	//	//g_env->DeleteLocalRef(cls);
	//}
	//else
	//{
	//	LOGDEBUG("N");
	//}

	if (!g_env)
	{
		clean_up(2);
		return 0;
	}

	// Instantiate hooks 
	HSwapBuffers::GetInstance();
	HWSASend::GetInstance();
	HWSARecv::GetInstance();

	LOGDEBUG("[init] Initializing hooks");
	Hook::InitializeAllHooks();

	LOGDEBUG("[init] Enabling hooks");
	Hook::EnableAllHooks();

	if (toad::g_curr_client == toad::MC_CLIENT::NOT_UPDATED)
	{
		clean_up(44);
		return 0;
	}
	if (toad::g_curr_client == toad::MC_CLIENT::NOT_SUPPORTED)
	{
		clean_up(45, "Client is not supported");
		return 0;
	}

	LOGDEBUG("[init] Client type {}", static_cast<int>(toad::g_curr_client));

	auto mcclass = Minecraft::findMcClass(g_env);
	if (mcclass == nullptr)
	{
		clean_up(4);
		return 0;
	}
	//auto mid = g_env->GetStaticMethodID(mcclass, "A", "()Lave;");
	//static bool once = false;
	//if (!once)
	//{
	//	jint count = 0;
	//	unsigned char* bytes;
	//	g_jvmti_env->GetBytecodes(mid, &count, &bytes);
	//	for (int i = 0; i < count; i++)
	//	{
	//		LOGDEBUG("{} {}", i, bytes[i]);
	//	}
	//	g_jvmti_env->Deallocate(bytes);
	//	once = true;
	//}

	auto eclasstemp = findclass("net.minecraft.entity.Entity", g_env);
	if (eclasstemp == nullptr)
	{
		clean_up(6);
		return 0;
	}

	LOGDEBUG("[init] mappings");
	//toad::g_curr_client = toad::MC_CLIENT::Forge;
	mappings::init_map(g_env, mcclass, eclasstemp, toad::g_curr_client);

	g_env->DeleteLocalRef(eclasstemp);
	g_env->DeleteLocalRef(mcclass);

	//MappingGenerator::Generate(g_env, g_jvmti_env);

	g_is_running = true;

	LOGDEBUG("[init] Starting modules");
	init_modules();

	LOGDEBUG("[init] Entering main loop");
	while (g_is_running)
	{
		if (GetAsyncKeyState(VK_END)) 
			break;

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
		
		CVarsUpdater::IsVerifiedCV.notify_all();
		for (auto& m : CModule::ModuleInstances)
		{
			m->EnabledCV.notify_one();
		}

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
		if (Tupdate_settings.joinable()) 
			Tupdate_settings.join();

		for (auto& [module_thread, name] : cmodule_threads)
		{
			LOGDEBUG("thread: {}", name);
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
		LOGWARN("Opening mapping file returned null");
		g_is_running = false;
		return false;
	}

	const auto pMem = MapViewOfFile(hMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, ipc_bufsize);
	if (pMem == NULL)
	{
		LOGWARN("Failed to map view of mapping file");
		CloseHandle(hMapFile);
		return false;
	}

	std::string s = (LPCSTR)pMem;
	//log_Debug(s.c_str());
	// parse buf as json and read them and set them and 

	std::string::size_type endof = s.find("END");
	if (endof == std::string::npos)
	{
		LOGERROR("Failed to find END, {}", endof);
		return false;
	}
	std::string settings = s.substr(0, endof);
	//log_Debug(settings.c_str());
	json data;
	try
	{
		data = json::parse(settings);
	}
	catch (json::parse_error& e)
	{
		LOGERROR("Json parse error: {} at: {}", e.what(), e.byte);
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
		esp::font_path = data["esp_font"];
		HSwapBuffers::UpdateFont();
		data["done"] = 0;
	}

	if (data.contains("path"))
	{
		loader_path = data["path"];
		data["donepath"] = 0;
	}

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

		if (open_menu_once_flag && g_is_ui_internal)
		{
			CInternalUI::MenuIsOpen = true;
			open_menu_once_flag = false;
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
	loaded_config = data["config"];

	std::string error_msg;
	if (!config::LoadSettings(data.dump(), error_msg))
	{
		LOGERROR("Error loading settings: {}", error_msg);
		return false;
	}

	CLeftAutoClicker::GetInstance()->UpdateEnabledState();
	CRightAutoClicker::GetInstance()->UpdateEnabledState();
	CVelocity::GetInstance()->UpdateEnabledState();
	CBridgeAssist::GetInstance()->UpdateEnabledState();
	CAimAssist::GetInstance()->UpdateEnabledState();
	CChestStealer::GetInstance()->UpdateEnabledState();
	CNoClickDelay::GetInstance()->UpdateEnabledState();
	CBlockEsp::GetInstance()->UpdateEnabledState();
	CEsp::GetInstance()->UpdateEnabledState();

	CLeftAutoClicker::SetDelays(left_clicker::min_cps, left_clicker::max_cps);
	CRightAutoClicker::SetDelays(right_clicker::cps);

	UnmapViewOfFile(pMem);
	CloseHandle(hMapFile);

	return true;
}

void init_modules()
{
	// Instantiate all cheat modules
	CVarsUpdater::GetInstance()->Name = "Vars updater";
	CLeftAutoClicker::GetInstance()->Name = "Left autoclicker";
	CRightAutoClicker::GetInstance()->Name = "Right autoclicker";
	CAimAssist::GetInstance()->Name = "Aim assist";
	CEsp::GetInstance()->Name = "Esp";
	CBlockEsp::GetInstance()->Name = "Block esp";
	CVelocity::GetInstance()->Name = "Velocity";
	CBlink::GetInstance()->Name = "Blink";
	CInternalUI::GetInstance()->Name = "Internal ui";
	CBridgeAssist::GetInstance()->Name = "Auto bridge";
	CNoClickDelay::GetInstance()->Name = "No Click Delay";
	CChestStealer::GetInstance()->Name = "Chest Stealer";

	//CReach::GetInstance()->name = "Reach";
	//COfScreenArrows::GetInstance()->name = "Off Screen Arrow ESP";

	// don't create threads for these modules
	CInternalUI::GetInstance()->IsOnlyRendering = true;

	for (const auto& Module : CModule::ModuleInstances)
	{
		if (Module->IsOnlyRendering)
		{
			continue;
		}

		LOGDEBUG("Starting cheat module: {}", Module->Name);

		cmodule_threads.emplace_back([&]
			{
				JNIEnv* env = nullptr;

				g_jvm->GetEnv(reinterpret_cast<void**>(&env), g_env->GetVersion());
				g_jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env), nullptr);

				auto mc = std::make_unique<Minecraft>();

				mc->env = env;

				Module->SetEnv(env);
				Module->SetMC(mc);

				Module->Initialized = true;

				while (g_is_running)
				{
					Module->PreUpdate();

					Module->Update(CVarsUpdater::theLocalPlayer);
				}

				LOGDEBUG("Closing cheat module: {}", Module->Name);
				g_jvm->DetachCurrentThread();
			}, 
			Module->Name);
	}

	// wait for all modules to be initalized before updating 
	std::ranges::borrowed_iterator_t<std::vector<CModule*>&> it;
	do
	{
		it = std::ranges::find_if(CModule::ModuleInstances, [](const auto& mod) { return !mod->Initialized; });

	} while (it != CModule::ModuleInstances.end());

}