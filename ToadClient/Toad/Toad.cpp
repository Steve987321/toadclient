#include "pch.h"
#include "Toad.h"

#include "nlohmann/json.hpp"

#include "../../Loader/src/Application/config.h"
#include "../../Loader/src/Application/config.cpp"

using namespace toadll;

// for getting settings from loader
constexpr static size_t bufsize = 3000;
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

	if (g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&g_env), nullptr) != JNI_OK)
	{
		clean_up(5, "failed to attach current thread");
		return 1;
	}

	if (g_jvm->GetEnv((void**)&g_jvmti_env, JVMTI_VERSION_1) == JNI_OK)
	{

		// check capabilities first 
		//jvmtiCapabilities capa;
		//if (g_jvmti_env->GetCapabilities(&capa) == JVMTI_ERROR_NONE)
		//{
		//	LOGDEBUG("can_get_bytecodes {}", capa.can_get_bytecodes);
		//	LOGDEBUG("can_tag_objects {}", capa.can_tag_objects);
		//	//LOGDEBUG("can_maintain_original_method_order {}", capa.can_maintain_original_method_order);

		//	//if (!capa.can_get_bytecodes || !capa.can_tag_objects)
		//		//clean_up(6, "no good jvm capabilities");
		//}

		//jint n = 0;
		//jclass* classes = nullptr;
		//g_jvmti_env->GetLoadedClasses(&n, &classes);
		//LOGDEBUG("classes count {}", n);

		//auto cls = findclass("java/lang/Class", g_env);
		//auto mId = g_env->GetMethodID(cls, "getName", "()Ljava/lang/String;");
		//
		//do 
		//{
		//	if (!cls)
		//		break;

		//	if (!mId)
		//	{
		//		g_env->DeleteLocalRef(cls);
		//		break;
		//	}

		//	for (jint i = 0; i < n; i++)
		//	{
		//		auto klass = classes[i];

		//		if (!klass)
		//			continue;

		//		const auto jstrname = static_cast<jstring>(g_env->CallObjectMethod(klass, mId));
		//		if (!jstrname)
		//		{
		//			LOGERROR("jstrname = nit shjicyed");
		//			continue;
		//		}
		//				
		//		//jfieldID* fields = nullptr;
		//		//jint field_count = -1;
		//		jmethodID* methods = nullptr;
		//		jint method_count = -1;
		//		//jint modifiers = -1;
		//		//g_jvmti_env->GetClassModifiers(klass, &modifiers);
		//		g_jvmti_env->GetClassMethods(klass, &method_count, &methods);
		//		//g_jvmti_env->GetClassFields(klass, &method_count, &fields);

		//		bool f = false;
		//		if (methods)
		//		{
		//			for (jint j = 0; j < method_count; j++)
		//			{
		//				char* name = nullptr;
		//				char* signature = nullptr;
		//				char* generic = nullptr;

		//				auto method = methods[j];
		//				if (!method)
		//					continue;

		//				auto err = g_jvmti_env->GetMethodName(method, &name, &signature, &generic);
		//				if (err != JVMTI_ERROR_NONE)
		//					continue;

		//				std::string name_str;
		//				if (name)
		//					name_str = name;

		//				if (name_str == "getMinecraft")
		//				{
		//					/*jboolean is_obselete = false;
		//					jint access_flags = -1;

		//					auto err1 = g_jvmti_env->IsMethodObsolete(methods[j], &is_obselete);
		//					auto err2 = g_jvmti_env->GetMethodModifiers(methods[j], &access_flags);
		//					bool is_static = false;

		//					if (err2 == JVMTI_ERROR_NONE)
		//					{
		//						is_static = access_flags & 0x0008;
		//					}
		//					*/
		//					LOGDEBUG("Found getmc: name: {} | sig: {} | generic: {} | count {}",
		//						name_str,
		//						signature ? signature : "None",
		//						generic ? generic : "None",
		//						(int)method_count
		//					);
		//					f = true;
		//					break;
		//				}

		//				g_jvmti_env->Deallocate((unsigned char*)name);
		//				g_jvmti_env->Deallocate((unsigned char*)signature);
		//				g_jvmti_env->Deallocate((unsigned char*)generic);
		//			}
		//		}

		//		if (auto err = g_jvmti_env->Deallocate((unsigned char*)methods); err != JVMTI_ERROR_NONE)
		//		{
		//			LOGERROR("Failed to deallocate methods array: {}", (int)err);
		//		}
		//	/*	if (auto err = g_jvmti_env->Deallocate((unsigned char*)fields); err != JVMTI_ERROR_NONE)
		//		{
		//			LOGERROR("Failed to deallocate fields array: {}", (int)err);
		//		}*/

		//		/*LOGDEBUG("{} | fields: {} methods: {} modifiers: {}", jstring2string(jstrname, g_env).c_str(), field_count, method_count, modifiers);*/

		//		if (f)
		//			break;
		//	}

		//} while (false);

		//g_jvmti_env->Deallocate((unsigned char*)classes);
		//g_env->DeleteLocalRef(cls);
	}
	else
	{
		LOGDEBUG("N");
	}


	if (!g_env)
	{
		clean_up(2);
		return 0;
	}

	// Instantiate hooks 
	HSwapBuffers::GetInstance();
	HWSASend::GetInstance();
	HWSARecv::GetInstance();

	LOGDEBUG("Initializing hooks");
	Hook::InitializeAllHooks();

	LOGDEBUG("Enabling hooks");
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

	LOGDEBUG("client type {}", static_cast<int>(toad::g_curr_client));

	auto mcclass = Minecraft::findMcClass(g_env);
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

	g_is_running = true;

	LOGDEBUG("starting modules");
	init_modules();

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
		if (g_jvm)
		{
			g_jvm->DetachCurrentThread();
		}

		if (g_jvmti_env)
		{
			g_jvmti_env->DisposeEnvironment();
		}

		LOGDEBUG("threads");
		if (Tupdate_settings.joinable()) Tupdate_settings.join();

		for (auto& ModuleThread : cmodule_threads)
			if (ModuleThread.joinable()) ModuleThread.join();

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

	const auto pMem = MapViewOfFile(hMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, bufsize);
	if (pMem == NULL)
	{
		LOGWARN("Failed to map view of mapping file");
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


	//// left auto clicker
	//left_clicker::enabled = data["lc_enabled"];
	//left_clicker::min_cps = data["lc_mincps"];
	//left_clicker::max_cps = data["lc_maxcps"];
	//left_clicker::break_blocks = data["lc_breakblocks"];
	//left_clicker::block_hit = data["lc_blockhit"];
	//left_clicker::block_hit_ms = data["lc_blockhitms"];
	//left_clicker::block_hit_stop_lclick = data["lc_blockhitpause"];
	//left_clicker::targeting_affects_cps = data["lc_smartcps"];
	//left_clicker::weapons_only = data["lc_weaponsonly"];
	//left_clicker::trade_assist = data["lc_tradeassist"];
	//left_clicker::start_break_blocks_reaction = data["lc_bbStart"];
	//left_clicker::stop_break_blocks_reaction = data["lc_bbStop"];

	//// right auto clicker
	//right_clicker::enabled = data["rc_enabled"];
	//right_clicker::cps = data["rc_cps"];
	//right_clicker::blocks_only = data["rc_blocks_only"];
	//right_clicker::start_delayms = data["rc_start_delay"];

	//// aim assist
	//aa::enabled = data["aa_enabled"];
	//aa::distance = data["aa_distance"];
	//aa::speed = data["aa_speed"];
	//aa::horizontal_only = data["aa_horizontal_only"];
	//aa::fov = data["aa_fov"];
	//aa::invisibles = data["aa_invisibles"];
	//aa::target_mode = data["aa_mode"];
	//aa::always_aim = data["aa_always_aim"];
	//aa::aim_at_closest_point = data["aa_multipoint"];
	//aa::lock_aim = data["aa_lockaim"];
	//aa::break_blocks = data["aa_bb"];

	//// no click delay
	//no_click_delay::enabled = data["ncd_enabled"];

	//// bridge assist
	//bridge_assist::enabled = data["ba_enabled"];
	//bridge_assist::pitch_check = data["ba_pitch_check"];
	//bridge_assist::block_check = data["ba_block_check"];
	//bridge_assist::only_initiate_when_sneaking = data["ba_iws"];

	//// blink
	//blink::enabled = data["bl_enabled"];
	//blink::key = data["bl_key"];
	//blink::stop_rec_packets = data["bl_stop_incoming_packets"];
	//blink::show_trail = data["bl_show_trail"];
	//blink::limit_seconds = data["bl_limit_seconds"];

	//// velocity
	//velocity::enabled = data["vel_enabled"];
	//velocity::jump_reset = data["vel_jumpreset"];
	//velocity::horizontal = data["vel_horizontal"];
	//velocity::vertical = data["vel_vertical"];
	//velocity::chance = data["vel_chance"];
	//velocity::delay = data["vel_delay"];

	//velocity::enabled = data["vel_enabled"];
	//velocity::only_when_clicking = data["vel_onlyclicking"];
	//velocity::only_when_moving = data["vel_onlymoving"];
	//velocity::kite = data["vel_kite"];
	//velocity::jump_reset = data["vel_jumpreset"];
	//velocity::jump_press_chance = data["vel_jumpchance"];
	//velocity::horizontal = data["vel_horizontal"];
	//velocity::vertical = data["vel_vertical"];
	//velocity::chance = data["vel_chance"];
	//velocity::delay = data["vel_delay"];

	//// ui
	//ui::show_array_list = data["ui_list"];
	//ui::show_water_mark = data["ui_mark"];

	//// esp
	//esp::enabled = data["esp_enabled"];
	//esp::line_col[0] = data["esp_linecolr"];
	//esp::line_col[1] = data["esp_linecolg"];
	//esp::line_col[2] = data["esp_linecolb"];
	//esp::line_col[3] = data["esp_linecola"];
	//esp::fill_col[0] = data["esp_fillcolr"];
	//esp::fill_col[1] = data["esp_fillcolg"];
	//esp::fill_col[2] = data["esp_fillcolb"];
	//esp::fill_col[3] = data["esp_fillcola"];
	//esp::text_bg_col[0] = data["esp_bgcolr"];
	//esp::text_bg_col[1] = data["esp_bgcolg"];
	//esp::text_bg_col[2] = data["esp_bgcolb"];
	//esp::text_bg_col[3] = data["esp_bgcola"];
	//esp::show_name = data["esp_show_name"];
	//esp::show_distance = data["esp_show_distance"];
	//esp::show_health = data["esp_show_health"];
	//esp::show_sneaking = data["esp_show_sneak"];
	//esp::esp_mode = data["esp_mode"];
	//esp::show_txt_bg = data["esp_bg"];

	//// esp extra
	//esp::static_esp_width = data["esp_static_width"];
	//esp::text_shadow = data["esp_text_shadow"];
	//esp::text_col[0] = data["esp_text_colr"];
	//esp::text_col[1] = data["esp_text_colg"];
	//esp::text_col[2] = data["esp_text_colb"];
	//esp::text_col[3] = data["esp_text_cola"];
	//esp::text_size = data["esp_fontsize"];
	//esp::show_border = data["esp_border"];
	//esp::custom_font = data["esp_text_font"];

	config::LoadSettings(data.dump());

	// block esp
	/*block_esp::enabled = data["blockesp_enabled"];
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

	block_esp::block_list = tmpList;*/

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
	//CReach::GetInstance()->name = "Reach";
	//COfScreenArrows::GetInstance()->name = "Off Screen Arrow ESP";

	// don't create threads for these modules
	CInternalUI::GetInstance()->IsOnlyRendering = true;

	for (const auto& Module : CModule::moduleInstances)
	{
		if (Module->IsOnlyRendering)
		{
			continue;
		}

		LOGDEBUG("Starting cheat module: {}", Module->Name);

		cmodule_threads.emplace_back([&]()
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

				LOGDEBUG("Closing cheat module: {}", Module->Name);
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