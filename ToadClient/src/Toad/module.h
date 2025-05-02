#pragma once

#include <utility>

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"
#include "toad/config.h"

namespace toad {

///
/// Interface for Cheat Modules
///
class CModule
{
public:
	CModule(std::string_view name, const bool& enabled, bool is_exposed = true, bool only_rendering = false)
		: Name(name), Enabled(enabled), IsExposed(is_exposed), IsOnlyRendering(only_rendering)
	{
	}

public:
	inline static std::vector<CModule*> ModuleInstances = {};
	std::condition_variable EnabledCV;

	std::string Name; 
	// will skip creating a thread for this module on initialization
	bool IsOnlyRendering = false;
	bool Initialized = false;
	const bool& Enabled;
	bool EnabledPrev = false;
	bool IsExposed = false;

public:
	static void AddModule(CModule* mod);
	static void UpdateModuleEnableStates();

	void SetEnv(JNIEnv* Env);

	/// Moves a newly made unique instance of Minecraft to this Cheat Module
	void SetMC(Minecraft& mc);

	void UpdateEnabledState();

public:
	/// Executes in a loop or 100ms(when not in game) even when player is null
	///	Calls sleep for 5ms
	virtual void PreUpdate();

	/// Executes in a loop when player is not null
	virtual void Update(const std::shared_ptr<LocalPlayer>& lPlayer);

	/// Executes inside the wglswapbuffers hook.
	///
	///	@see HSwapBuffers
	virtual void OnRender();

	/// Executes inside the wglswapbuffers hook when ImGui is getting rendered.
	///
	///	@see HSwapBuffers
	virtual void OnImGuiRender(ImDrawList* draw);

	template<typename T>
	static T* GetInstance(std::string_view name, const bool& enabled, bool is_exposed = true, bool only_rendering = false)
	{
		static T instance(name, enabled, is_exposed, only_rendering);
		return &instance;
	}

protected:
	void WaitIsVerified();
	void WaitIsEnabled();
	
protected:
	std::mutex verified_mutex;
	std::mutex enabled_mutex;
	std::mutex enabled_update_mutex;

	JNIEnv* env = nullptr;
	Minecraft* MC = nullptr;
};

}

#define REGISTER_CMODULE(TMOD, ...) CModule::AddModule(TMOD::GetInstance<TMOD>(__VA_ARGS__))
