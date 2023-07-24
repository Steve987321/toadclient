#pragma once

#include <d3d9.h>
#include <d3d9types.h>

namespace toad {

///
/// Class that manages windows with ImGui
///
class Window final
{
public:
	Window(const std::string& window_title, int win_height, int win_width);
	~Window();

public:
	struct D3DProperties
	{
		LPDIRECT3D9              pD3D{};
		LPDIRECT3DDEVICE9        pD3DDevice{};
		D3DPRESENT_PARAMETERS    pD3DParams{};
	};

public:
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/// Returns an active window with given argument as id if found.
	///
	///	Returns null if not found
	static Window* GetWindowInstance(std::string_view window_name);
	static Window* GetWindowInstance(const HWND& hwnd);

	D3DProperties* GetD3DProperties();
	HWND GetHandle() const;
	bool IsActive() const;

public:
	void SetUI(const std::function<void(ImGuiIO* io)>& ui_func);

public:
	void UpdateMenu();

private:
	void DestroyWindow();
	void StartImGuiWindow(const std::string& window_title, int win_height, int win_width);

private:
	bool CreateDeviceD3D();
	void CleanupDeviceD3D();
	void ResetDevice();

private:
	std::string m_windowName;
	std::string m_windowClassName;

	std::atomic_bool m_isRunning = false;
	std::atomic_bool m_isInvalid = false;

	inline static std::unordered_map<std::string, Window*> m_windowNameMap = {};
	inline static std::unordered_map<HWND, Window*> m_windowHwndMap = {};

	std::function<void(ImGuiIO* io)> m_uiFunction = {};

	std::thread m_windowThread; 

	HWND m_hwnd = {};
	WNDCLASSEXA m_wc = {};

	D3DProperties m_d3dProperties = {};

	// imgui
	ImGuiIO* m_io = {};
	ImGuiStyle* m_style = {};

};

}
