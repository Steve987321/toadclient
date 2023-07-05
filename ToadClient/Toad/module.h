#pragma once

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

namespace toadll {

/**
 * @brief
 * interface for cheat features
 */
	class CModule
	{
	protected:
		JNIEnv* env = nullptr;

		std::shared_ptr<c_Minecraft> Minecraft = nullptr;

	public:
		bool Initialized = false;
		inline static std::vector<CModule*> moduleInstances = {};

	public:
		CModule()
		{
			moduleInstances.emplace_back(this);
		}

	public:
		void SetEnv(JNIEnv* Env) { env = Env; }
		void SetMC(std::shared_ptr<c_Minecraft> mc) { Minecraft = mc; }

	public:
		// Executes every tick or 100ms(when not in game) even when player is null
		virtual void PreUpdate() { SLEEP(1); }

		// Executes every system tick when player is not null
		virtual void Update(const std::shared_ptr<LocalPlayerT>& lPlayer) { SLEEP(1); }

		// Executes inside the wglswapbuffers hook 
		virtual void OnRender() {}

		// Executes inside the wglswapbuffers hook when ImGui is getting rendered
		virtual void OnImGuiRender(ImDrawList* draw) {}

};

}

