#include "pch.h"
#include "Toad.h"

//#include <glew-2.1.0/include/GL/glew.h>
//#include <gl/GL.h>

int stage = 0;

namespace toadll
{
	void init()
	{
#ifdef _DEBUG
		p_Log = std::make_unique<c_Logger>();
#endif

		// get functions from jvm.dll
		auto jvmHandle = GetModuleHandleA("jvm.dll");

		oJNI_GetCreatedJavaVMs = reinterpret_cast<hJNI_GetCreatedJavaVMs>(GetProcAddress(jvmHandle, "JNI_GetCreatedJavaVMs"));
		oJVM_GetMethodIxNameUTF = reinterpret_cast<hJVM_GetMethodIxNameUTF>(GetProcAddress(jvmHandle, "JVM_GetMethodIxNameUTF"));
		oJVM_GetMethodIxSignatureUTF = reinterpret_cast<hJVM_GetMethodIxSignatureUTF>(GetProcAddress(jvmHandle, "JVM_GetMethodIxSignatureUTF"));
		oJVM_GetClassMethodsCount = reinterpret_cast<hJVM_GetClassMethodsCount>(GetProcAddress(jvmHandle, "JVM_GetClassMethodsCount"));
		oJVM_GetClassFieldsCount = reinterpret_cast<hJVM_GetClassFieldsCount>(GetProcAddress(jvmHandle, "JVM_GetClassFieldsCount"));
		oJVM_GetClassDeclaredFields = reinterpret_cast<hJVM_GetClassDeclaredFields>(GetProcAddress(jvmHandle, "JVM_GetClassDeclaredFields"));
		oJVM_GetArrayElement = reinterpret_cast<hJVM_GetArrayElement>(GetProcAddress(jvmHandle, "JVM_GetArrayElement"));
		oJVM_GetArrayLength = reinterpret_cast<hJVM_GetArrayLength>(GetProcAddress(jvmHandle, "JVM_GetArrayLength"));
		oJVM_GetMethodIxArgsSize = reinterpret_cast<hJVM_GetMethodIxArgsSize>(GetProcAddress(jvmHandle, "JVM_GetMethodIxArgsSize"));

		oJNI_GetCreatedJavaVMs(&jvm, 1, 0);
		jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), NULL);

		if (!env)
			clean_up();

		p_Minecraft = std::make_unique<c_Minecraft>();

		if (!p_Minecraft->init()) 
			clean_up();

		mappings::init_map(curr_client);

		is_running = true;

		while (is_running)
		{
			update();
			if (GetAsyncKeyState(VK_END)) break;
		}
		clean_up();

		//if (MH_Initialize() != MH_OK)
		//	; // do something 

		//auto AddressToHook = reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers"));

		//MH_CreateHookApi(L"opengl32.dll", "wglSwapBuffers", nullptr, nullptr); // use this api hook?  instead of normal ?  

	}

	// main loop
	void update()
	{
		auto player = p_Minecraft->get_localplayer();
		c_Entity entity(&player);
		std::cout << "X: " << entity.get_position().x << " Y: " << entity.get_position().y << " Z: " << entity.get_position().z << std::endl;
	}

	void clean_up()
	{
		if (jvm != nullptr)
		{
			jvm->DetachCurrentThread();
			jvm->DestroyJavaVM();
		}

		env = nullptr;
		jvm = nullptr;

		p_Log->dispose_console();
		//FreeConsole();
		FreeLibraryAndExitThread(hMod, 0);
		//MH_Uninitialize();
	}
}
