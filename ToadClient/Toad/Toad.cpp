 #include "pch.h"
#include "Toad.h"

#include "nlohmann/json.hpp"

//#include <glew-2.1.0/include/GL/glew.h>
//#include <gl/GL.h>

constexpr static size_t bufsize = 1000;
int stage = 0;

namespace toadll
{
	void Fupdate_settings()
	{
		while (is_running)
		{
			HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"ToadClientMappingObj");
			if (hMapFile == NULL)
			{
				is_running = false;
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

			// aim assist
			aa::enabled = data["aaenabled"];
			aa::distance = data["aadistance"];
			aa::speed = data["aaspeed"];
			aa::horizontal_only = data["aahorizontal_only"];
			aa::fov = data["aafov"];
			aa::invisibles = data["aainvisibles"];
			aa::targetFOV = data["aatargetFOV"];
			aa::always_aim = data["aaalways_aim"];

			// auto bridge
			auto_bridge::enabled = data["abenabled"];
			auto_bridge::pitch_check = data["abpitch_check"];

			// velocity
			velocity::enabled = data["velenabled"];
			velocity::horizontal = data["velhorizontal"];
			velocity::vertical = data["velvertical"];
			velocity::chance = data["velchance"];
			velocity::delay = data["veldelay"];

			UnmapViewOfFile(buf);
			CloseHandle(hMapFile);

			SLOW_SLEEP(100);
		}
	}

	DWORD WINAPI init()
	{
		p_Log = std::make_unique<c_Logger>();
		SetConsoleCtrlHandler(NULL, true);

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

		jvmfunc::oJNI_GetCreatedJavaVMs(&jvm, 1, nullptr);

		if (jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK)
			return 1;

		if (!env)
		{
			clean_up(2);
			return 0;
		}

		auto mcclass = p_Minecraft->get_mcclass();

		p_Minecraft = std::make_unique<c_Minecraft>(mcclass);


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

		auto eclasstemp = findclass("net.minecraft.entity.Entity");
		if (eclasstemp == nullptr)
		{
			clean_up(6);
			return 0;
		}
		mappings::init_map(env, mcclass, eclasstemp, curr_client);

		env->DeleteLocalRef(eclasstemp);

		is_running = true;

		// threads
		Tupdate_settings = std::thread([] { Fupdate_settings(); });
		Tupdate_cursorinfo = std::thread([]
			{
				while (is_running)
				{
					CURSORINFO ci{ sizeof(CURSORINFO) };
					if (GetCursorInfo(&ci))
					{
						auto handle = reinterpret_cast<int>(ci.hCursor);
						is_cursor_shown = (handle) > 100000 && (handle) < 1000000 || (handle) == 13961697 ? false : true;
					}
				}
			});

		log_Debug("enabling hooks");

		// swapbuffers
		c_Swapbuffershook::get_instance()->enable();
		c_WSASend::get_instance()->enable();
		// wsasend & wsarecv
		

		std::cout << "main loop starting\n";
		SLOW_SLEEP(1000);

		// main loop
		while (is_running)
		{
			modules::update();
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
			is_running = false;
			log_Debug("closing: %d", exitcode);
			if (!c_Swapbuffershook::get_instance()->is_null())
				c_Swapbuffershook::get_instance()->dispose();

			if (!c_WSASend::get_instance()->is_null())
				c_WSASend::get_instance()->dispose();

			log_Debug("jvm");
			if (jvm != nullptr)
			{
				jvm->DetachCurrentThread();
			}

			log_Debug("threads");
			if (Tupdate_settings.joinable()) Tupdate_settings.join();
			if (Tupdate_cursorinfo.joinable()) Tupdate_cursorinfo.join();
			if (Tupdate_hookvars.joinable()) Tupdate_hookvars.join();

			env = nullptr;
			jvm = nullptr;

			log_Debug("console");
			p_Log->dispose_console();

			FreeLibraryAndExitThread(hMod, 0);
			});
	}
}
