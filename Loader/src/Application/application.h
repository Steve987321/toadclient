#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "Fonts/icons.h"

#include "imgui_window.h"

namespace toad
{

class Application
{
public:
	Application();
	~Application();

public:
	bool Init();
	void MainLoop();
	void Exit() const;

	static Application* Get();
public:
	std::shared_ptr<ImGuiWindow> GetWindow() const
	{
		return m_window;
	}

public:
	constexpr static int WINDOW_WIDTH = 500, WINDOW_HEIGHT = 400;

private:
	inline static Application* s_instance = nullptr;
	std::shared_ptr<ImGuiWindow> m_window = nullptr;

private:
	static void render_UI(ImGuiIO* io);
};

}
