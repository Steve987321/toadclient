#pragma once

#include <d3d9.h>
#include <d3d9types.h>

namespace toad {

///
/// Class that manages windows with ImGui
///
class Window
{
public:
	Window(std::string_view window_title, int win_height, int win_width);
	~Window();

public:
	struct D3DProperties
	{
		LPDIRECT3D9              pD3D{};
		LPDIRECT3DDEVICE9        pD3DDevice{};
		D3DPRESENT_PARAMETERS    D3DParams{};
	};

public:
	/// Returns an active window with given argument as id if found.
	///
	///	Returns null if not found
	
	static Window* GetWindowInstance(std::string_view window_name);
	static Window* GetWindowInstance(const HWND& hwnd);

	D3DProperties& GetD3DProperties();

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


private:
	void DestroyWindow();
	bool CreateImGuiWindow(std::string_view window_title, int win_height, int win_width);

private:
	bool CreateDeviceD3D(const HWND& hWnd);
	void CleanupDeviceD3D();
	void ResetDevice();

private:
	void MenuLoop();

private:
	std::string m_windowName;
	bool m_isInvalid = false;

	inline static std::unordered_map<std::string, Window*> m_windowNameMap {};
	inline static std::unordered_map<HWND, Window*> m_windowHwndMap {};

	std::function<void()> m_uiFunction {};

	HWND m_hwnd {};
	WNDCLASSEX m_wc{};

	D3DProperties m_d3dProperties {};

	// imgui
	ImGuiIO* m_io{};
	ImGuiStyle* m_style{};

};

}
