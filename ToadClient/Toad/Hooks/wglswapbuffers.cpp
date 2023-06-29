#include "pch.h"
#include "Toad/Toad.h"
#include "wglswapbuffers.h"

#include "../Loader/src/Application/Fonts/Icons.h"
#include "../Loader/src/Application/Fonts/fa-solid-900Font.h"

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace toadll
{
	LONG_PTR WINAPI CSwapBuffers::WndProcHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_KEYDOWN)
		{
			if (wParam == CInternalUI::ShowMenuKey)
			{
				CInternalUI::MenuIsOpen = !CInternalUI::MenuIsOpen;
			}
		}
		
		if (toad::g_is_ui_internal && CInternalUI::MenuIsOpen && is_imgui_initialized)
		{
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
			return true;
		}

		return CallWindowProc(oWndProc, hwnd, msg, wParam, lParam);
	}

	BOOL CSwapBuffers::Hook(HDC hDc)
	{
		hwnd = WindowFromDC(hDc);
		if (!oWndProc)
		{
			oWndProc = reinterpret_cast<tWndProc>(SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));
		}

		HGLRC oCtx = wglGetCurrentContext();

		static HGLRC ctx = nullptr;
		static int init_stage = 0;

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		// check for resolution change
		if (screen_width != viewport[2] || screen_height != viewport[3])
		{
			// update window handle 
			// window handle gets lost when switching between fullscreen and windowed
			g_hWnd = hwnd;
		}

		screen_width = viewport[2];
		screen_height = viewport[3];

		for (const auto& Module : CModule::moduleInstances)
			Module->OnRender();

		if (init_stage == 0)
		{
			ctx = wglCreateContext(hDc);
			wglMakeCurrent(hDc, ctx);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glOrtho(0, viewport[2], viewport[3], 0, 1, -1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glClearColor(0, 0, 0, 1);

			init_stage = 1;
		}

		wglMakeCurrent(hDc, ctx);

		if (init_stage == 1)
		{
			ImGui::CreateContext();
			ImGui_ImplWin32_Init(hwnd);
			ImGui_ImplOpenGL2_Init();
			auto io = &ImGui::GetIO(); (void)io;
			io->ConfigWindowsMoveFromTitleBarOnly = true;
			io->Fonts->AddFontDefault();
			static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
			ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
			io->Fonts->AddFontFromMemoryCompressedBase85TTF(base85_compressed_data_fa_solid_900, 24, &icons_config, icons_ranges);

			ImGui::SetNextWindowSize({500, 500});
			is_imgui_initialized = true;
			init_stage = 3;
		}

		auto& io = ImGui::GetIO();
		io.DisplaySize = { static_cast<float>(screen_width), static_cast<float>(screen_height) };

		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		const auto draw = ImGui::GetBackgroundDrawList();

		for (const auto& Module : CModule::moduleInstances)
			Module->OnImGuiRender(draw);

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		wglMakeCurrent(hDc, oCtx);
		return owglSwapBuffers(hDc);
	}

	bool CSwapBuffers::Init()
	{
		return create_hook("opengl32.dll", "wglSwapBuffers", &CSwapBuffers::Hook, reinterpret_cast<LPVOID*>(&owglSwapBuffers));
	}

	void CSwapBuffers::Dispose()
	{
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));

		CHook::Dispose();
	}
}
