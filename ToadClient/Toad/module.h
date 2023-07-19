#pragma once

#include <utility>

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

namespace toadll {

///
/// Interface for Cheat Modules
///
class CModule
{
protected:
	JNIEnv* env = nullptr;

	std::shared_ptr<Minecraft> MC = nullptr;

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

	/// Sets a newly made unique instance of Minecraft to this Cheat Module
	void SetMC(std::unique_ptr<Minecraft>& mc) { MC = std::move(mc); }

public:
	/// Executes every tick or 100ms(when not in game) even when player is null
	virtual void PreUpdate() { SLEEP(1); }

	/// Executes every system tick when player is not null
	virtual void Update(const std::shared_ptr<LocalPlayer>& lPlayer) { SLEEP(1); }

	/// Executes inside the wglswapbuffers hook.
	///
	///	@see HSwapBuffers
	virtual void OnRender() {}

	/// Executes inside the wglswapbuffers hook when ImGui is getting rendered.
	///
	///	@see HSwapBuffers
	virtual void OnImGuiRender(ImDrawList* draw) {}

};

}

