#pragma once

namespace toadll
{

class HSwapBuffers SET_HOOK_CLASS(HSwapBuffers)
{
public:
	bool Init() override;
	void Dispose() override;

public:
	static void UpdateFont();
	static ImFont* GetFont();

private:
	static inline bool m_is_imgui_initialized = false;
	static inline HWND m_hwnd = nullptr;

	// font 
	static inline ImFont* m_esp_font = nullptr;
	static inline bool m_esp_font_update = false;

private:
	typedef LONG_PTR(CALLBACK* tWndProc) (HWND, UINT, WPARAM, LPARAM);
	typedef BOOL(WINAPI* twglSwapBuffers) (HDC hDc);

private:
	static inline tWndProc oWndProc = nullptr;
	static LONG_PTR CALLBACK WndProcHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	static inline twglSwapBuffers owglSwapBuffers = nullptr;
	static BOOL Hook(HDC hDc);
};

}