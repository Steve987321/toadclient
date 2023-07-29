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
	bool Initialized = false;
	inline static std::vector<CModule*> moduleInstances = {};

	// debugging purposes
	std::string name; 

public:
	CModule()
	{
		moduleInstances.emplace_back(this);
	}

public:
	void SetEnv(JNIEnv* Env);

	/// Moves a newly made unique instance of Minecraft to this Cheat Module
	void SetMC(std::unique_ptr<Minecraft>& mc);

public:
	/// Executes every tick or 100ms(when not in game) even when player is null
	virtual void PreUpdate();

	/// Executes every system tick when player is not null
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
	JNIEnv* env = nullptr;
	std::shared_ptr<Minecraft> MC = nullptr;

protected:
	// useful helper functions for cheat modules

	/// Returns the player list excluding the local player 
	_NODISCARD std::vector<Entity> GetPlayerList() const;
};

}

