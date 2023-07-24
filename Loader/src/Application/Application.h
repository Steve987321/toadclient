#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "Fonts/Icons.h"

#include "window.h"

namespace toad
{

class Application
{
public:
	_NODISCARD bool Init();
	void MainLoop();
	void Exit() const;

public:
	_NODISCARD std::shared_ptr<Window> GetWindow() const
	{
		return m_window;
	}

public:
	constexpr static int WINDOW_WIDTH = 500, WINDOW_HEIGHT = 400;

private:
	std::shared_ptr<Window> m_window = nullptr;

	void InitConsole();
	//bool SetupMenu();
	//void MenuLoop();

private:
	static void render_UI(ImGuiIO* io);
};
inline auto AppInstance = std::make_unique<Application>();

}
