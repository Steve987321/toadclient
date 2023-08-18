#pragma once

#include <d3d9.h>
#include <d3d9types.h>

namespace toad {

///
/// Class that manages windows with ImGui
///
/// TODO: Fix when creating multiple Windows 
///
class ImGuiWindow final
{
public:
	ImGuiWindow(std::string window_title, int win_height, int win_width);
	~ImGuiWindow();

public:
	struct D3DProperties
	{
		LPDIRECT3D9              pD3D{};
		LPDIRECT3DDEVICE9        pD3DDevice{};
		D3DPRESENT_PARAMETERS    pD3DParams{};
	};

public:
	void StartWindow();
	void DestroyWindow();

public:
	D3DProperties* GetD3DProperties();
	HWND GetHandle() const;
	bool IsActive() const;
	bool IsFontUpdated() const;

public:
	void SetUI(const std::function<void(ImGuiIO* io)>& ui_func);
	void AddFontTTF(std::string_view pathTTF);

public:
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/// Returns an active window with given argument as id if found.
	///
	///	Returns null if not found
	static ImGuiWindow* GetWindowInstance(std::string_view window_name);
	static ImGuiWindow* GetWindowInstance(const HWND& hwnd);

private:
	void CreateImGuiWindow(const std::string& window_title, int win_height, int win_width);

	/// The ui that displays if no UI has been set using SetUI
	static void DefaultUIWindow(ImGuiIO* io); 
private:
	bool CreateDeviceD3D();
	void CleanupDeviceD3D();
	void ResetDevice();

	void UpdateMenu();

private:
	std::string m_windowName;
	std::string m_windowClassName;

	int m_window_width, m_window_height;

	std::mutex m_destroyWindowMutex;

	std::atomic_bool m_isRunning = true;
	std::atomic_bool m_shouldClose = false;

	bool m_isUIFuncSet = false;

	bool m_update_font = false;
	std::string m_update_font_path;

	inline static std::unordered_map<std::string, ImGuiWindow*> m_windowNameMap = {};
	inline static std::unordered_map<HWND, ImGuiWindow*> m_windowHwndMap = {};

	std::function<void(ImGuiIO* io)> m_uiFunction = DefaultUIWindow;

	std::thread m_windowThread; 

	HWND m_hwnd = {};
	WNDCLASSEXA m_wc = {};

	D3DProperties m_d3dProperties = {};

	// imgui
	ImGuiIO* m_io = {};
	ImGuiStyle* m_style = {};

};

}
