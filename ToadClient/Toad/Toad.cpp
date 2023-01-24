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

		jvmfunc::oJNI_GetCreatedJavaVMs(&jvm, 1, 0);

		jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), NULL);

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

		mappings::init_map(curr_client);

		is_running = true;

		p_Hooks->enable();

		while (is_running)
		{
			//update();
			if (GetAsyncKeyState(VK_END)) break;
		}
		clean_up(0);
		
	}

	void update()
	{
		std::cout << "update:\n";
		auto entities = p_Minecraft->get_playerList();
		auto lplayer = std::make_unique<c_Entity>(std::make_unique<jobject>(p_Minecraft->get_localplayer()));
		for (const auto& player : entities)
		{
			std::cout << "X: " << player->get_position().x << " Y: " << player->get_position().y << " Z: " << player->get_position().z << std::endl;
		}
		/*auto player = p_Minecraft->get_localplayer();
		c_Entity entity(&player);
		std::cout << "X: " << entity.get_position().x << " Y: " << entity.get_position().y << " Z: " << entity.get_position().z << std::endl;
		*/
	}

	void clean_up(int exitcode)
	{
		log_Debug("closing: %d", exitcode);
		Sleep(1000);
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
		//MH_Uninitialize();
	}
}
