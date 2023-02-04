#pragma once

#include "imgui/imgui.h"

namespace toad
{

	class c_Application
	{
	private:
		HWND				     hwnd = {};
		RECT					 rect = {};
		WNDCLASSEX				 wc = {};

		ImGuiIO* io = nullptr;
		ImGuiStyle* style = nullptr;

		FILE* f = nullptr;

		// from imgui's example
		[[nodiscard]] static bool CreateDeviceD3D(const HWND& hWnd);
		static void CleanupDeviceD3D();
		static void ResetDevice();
		static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void InitConsole();
		bool SetupMenu();
		void MenuLoop();
		//void UpdateCursorInfo();

	private:
		void render_UI(ImGuiIO* io);

	public:
		constexpr static int WINDOW_WIDTH = 500, WINDOW_HEIGHT = 400;

	public:
		[[nodiscard]] bool Init();
		void Run();
		void Dispose() const;

	public:
		[[nodiscard]] HWND get_window() const
		{
			return this->hwnd;
		}
	};

inline std::unique_ptr<c_Application> p_App;

}
