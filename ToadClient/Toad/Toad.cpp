#include "pch.h"
#include "Toad.h"

#include "nlohmann/json.hpp"

//#include <glew-2.1.0/include/GL/glew.h>
//#include <gl/GL.h>

int stage = 0;

namespace toadll
{
	void update_settings()
	{
		constexpr static size_t bufsize = 1000;
		HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"Global\\Toad");
		if (hMapFile == NULL)
			clean_up(11);
		auto buf = (LPCSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, bufsize);
		if (buf == NULL)
		{
			clean_up(12);
			CloseHandle(hMapFile);
		}

		// parse buf as json and read them and set them and yes

		using json = nlohmann::json;

		json data = json::parse(buf);

		aa::enabled = data["aaenabled"];
		aa::distance = data["aadistance"];
		aa::speed = data["aaspeed"];

		UnmapViewOfFile(buf);
		CloseHandle(hMapFile);
	}

	void init()
	{
#ifdef _DEBUG
		p_Log = std::make_unique<c_Logger>();
#endif

		SetConsoleCtrlHandler(NULL, true);

		// get functions from jvm.dll
		auto jvmHandle = GetModuleHandleA("jvm.dll");

		if (jvmHandle == nullptr)
		{
			clean_up(1);
			return;
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

		jvmfunc::oJNI_GetCreatedJavaVMs(&jvm, 1, nullptr);

		jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr);

		if (!env)
		{
			clean_up(2);
			return;
		}

		p_Minecraft = std::make_unique<c_Minecraft>();
		p_Hooks = std::make_unique<c_Hooks>();

		if (!p_Hooks->init())
		{
			clean_up(3);
			return;
		}

		if (!p_Minecraft->init())
		{
			clean_up(4);
			return;
		}

		mappings::init_map(env, p_Minecraft->get_mcclass(), curr_client);

		is_running = true;

		p_Hooks->enable();

		while (is_running)
		{
			//update_settings();
			modules::update();
			if (GetAsyncKeyState(VK_END)) break;
		}
		clean_up(0);		
	}

	void clean_up(int exitcode)
	{
		log_Debug("closing: %d", exitcode);

		if (jvm != nullptr)
		{
			jvm->DetachCurrentThread();
			jvm->DestroyJavaVM();
		}

		if (p_Hooks != nullptr)
			p_Hooks->dispose();

		env = nullptr;
		jvm = nullptr;

		p_Log->dispose_console();
		FreeLibraryAndExitThread(hMod, 0);
	}
}
