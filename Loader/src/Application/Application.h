#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "Fonts/Icons.h"

namespace toad
{
	class Application
	{
	private:
		HWND				     m_hwnd = {};
		RECT					 m_rect = {};
		WNDCLASSEX				 m_wc = {};

		ImGuiIO* m_io = nullptr;
		ImGuiStyle* m_style = nullptr;

		FILE* f = nullptr;

		// from imgui's example
		_NODISCARD static bool CreateDeviceD3D(const HWND& hWnd);
		static void CleanupDeviceD3D();
		static void ResetDevice();
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void InitConsole();
		bool SetupMenu();
		void MenuLoop();

	private:
		// defined in ui.cpp
		void render_UI() const;

	public:
		constexpr static int WINDOW_WIDTH = 500, WINDOW_HEIGHT = 400;

	public:
		_NODISCARD bool Init();
		void MainLoop();
		void Exit() const;

	public:
		_NODISCARD HWND GetWindow() const
		{
			return this->m_hwnd;
		}
	};
	inline auto AppInstance = std::make_unique<Application>();
}
