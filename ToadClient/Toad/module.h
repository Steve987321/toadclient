#pragma once

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

namespace toadll {

//class CModule;

//inline std::vector<CModule*> moduleInstances = {};

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
	// Executes every system tick 
	virtual void Update(const std::shared_ptr<c_Entity>& lPlayer) {}

	// Executes every in game tick 
	virtual void OnTick(const std::shared_ptr<c_Entity>& lPlayer) {}

	// Executes inside the wglswapbuffers hook 
	virtual void OnRender() {}

};

}

