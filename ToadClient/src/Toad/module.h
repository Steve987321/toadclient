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
public:
	CModule();

public:
	inline static std::vector<CModule*> ModuleInstances = {};
	std::condition_variable EnabledCV;

	std::string Name; 

	// will skip creating a thread for this module on initialization
	bool IsOnlyRendering = false;
	bool Initialized = false;
	bool* Enabled = nullptr;
	bool EnabledPrev = false;

public:
	static std::vector<CModule*> GetEnabledModules();

	void SetEnv(JNIEnv* Env);

	/// Moves a newly made unique instance of Minecraft to this Cheat Module
	void SetMC(std::unique_ptr<Minecraft>& mc);

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

protected:
	void WaitIsVerified();
	void WaitIsEnabled();

protected:
	std::mutex verified_mutex;
	std::mutex enabled_mutex;
	std::mutex enabled_update_mutex;

	JNIEnv* env = nullptr;
	std::shared_ptr<Minecraft> MC = nullptr;
};

}

