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

		mappings::init_map(env, p_Minecraft->get_mcclass(), curr_client);

		is_running = true;

		p_Hooks->enable();

		//p_AimAssist->start_thread();

		while (is_running)
		{
			//update();
			if (GetAsyncKeyState(aa::key))
			{
				std::vector <std::pair<float, std::shared_ptr<c_Entity>>> distances = {};
				auto lPlayer = p_Minecraft->get_localplayer();
				auto lPlayername = lPlayer->get_name();

				for (const auto& player : p_Minecraft->get_playerList())
				{
					if (player->get_name() == lPlayername) continue;
					distances.emplace_back(lPlayer->get_position().dist(player->get_position()), player);
				}
				if (distances.size() < 2) continue;
				auto t = std::min_element(distances.begin(), distances.end());
				auto target = t->second;

				auto [yaw, pitch] = get_angles(lPlayer->get_position(), target->get_position());

				auto lyaw = lPlayer->get_rotationYaw();
				auto lpitch = lPlayer->get_rotationPitch();

				float difference = wrap_to_180(-(lyaw - yaw));
				float difference2 = wrap_to_180(-(lpitch - pitch));

				lPlayer->set_rotation(lyaw + difference / 10, lpitch + difference2 / 10);
			}
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
		//MH_Uninitialize();
	}
}
