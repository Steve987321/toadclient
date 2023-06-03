#pragma once

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

namespace toadll {

class CModule;

inline std::vector<CModule*> moduleInstances = {};

/**
 * @brief
 * interface for cheat features
 */
class CModule
{

public:
	CModule()
	{
		moduleInstances.emplace_back(this);
	}

public:
	// Executes every system tick 
	virtual void Update(const std::shared_ptr<c_Entity>& lPlayer) {}

	// Executes every in game tick 
	virtual void OnTick(const std::shared_ptr<c_Entity>& lPlayer) {}

	// Executes inside the wglswapbuffers hook 
	virtual void OnRender(ImDrawList* draw) {}

};

}

