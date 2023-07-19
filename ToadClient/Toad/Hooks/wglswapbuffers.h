#pragma once

namespace toadll
{

class HSwapBuffers SET_HOOK_CLASS(HSwapBuffers)
{
private:
	static inline bool is_imgui_initialized = false;
	static inline HWND hwnd = nullptr;

private:
	typedef LONG_PTR(CALLBACK* tWndProc) (HWND, UINT, WPARAM, LPARAM);
	typedef BOOL(WINAPI* twglSwapBuffers) (HDC hDc);

private:
	static inline tWndProc oWndProc = nullptr;
	static LONG_PTR CALLBACK WndProcHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	static inline twglSwapBuffers owglSwapBuffers = nullptr;
	static BOOL Hook(HDC hDc);

public:
	bool Init() override;
	void Dispose() override;
};

}