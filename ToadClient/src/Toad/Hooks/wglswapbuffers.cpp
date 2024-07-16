#include "pch.h"
#include "Toad/toadll.h"
#include "wglswapbuffers.h"

#include <array>

#include "../Loader/src/Application/Fonts/icons.h"
#include "../Loader/src/Application/Fonts/fa-solid-900Font.h"

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

namespace toadll
{
	LONG_PTR WINAPI HSwapBuffers::WndProcHook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_KEYDOWN)
		{
			if (wParam == CInternalUI::ShowMenuKey)
			{
				CInternalUI::MenuIsOpen = !CInternalUI::MenuIsOpen;

				static POINT cursorPos { -1, -1 };
				if (CInternalUI::MenuIsOpen)
				{
					// opened the menu 

					// save cursor pos
					GetCursorPos(&cursorPos);
				}
				else
				{
					// closed the menu

					// set cursor pos to original position to prevent the flicking
					if (cursorPos.x != -1 || cursorPos.y != -1)
						SetCursorPos(cursorPos.x, cursorPos.y);
				}
			}
		}
		
		if (GetForegroundWindow() == hwnd && toad::g_is_ui_internal && CInternalUI::MenuIsOpen && m_is_imgui_initialized)
		{
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
			return true;
		}
		return CallWindowProc(oWndProc, hwnd, msg, wParam, lParam);
	}

	BOOL HSwapBuffers::Hook(HDC hDc)
	{
		m_hwnd = WindowFromDC(hDc);

		if (!oWndProc)
		{
			oWndProc = reinterpret_cast<tWndProc>(SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));
		}

		HGLRC oCtx = wglGetCurrentContext();

		static HGLRC ctx = nullptr;
		static int init_stage = 0;

		glGetIntegerv(GL_VIEWPORT, CVarsUpdater::Viewport.data());

		// check for viewport dimensions change
		if (g_screen_width != CVarsUpdater::Viewport[2] || g_screen_height != CVarsUpdater::Viewport[3])
		{
			RECT r;
			GetWindowRect(GetDesktopWindow(), &r);

			if (init_stage != 0)
			{
				if ((CVarsUpdater::Viewport[2] == r.right && CVarsUpdater::Viewport[3] == r.bottom)
					||
					(g_screen_width == r.right && g_screen_height == r.bottom))
				{
					LOGDEBUG("[HSwapBuffers] Switching fullscreen");

					ImGui_ImplWin32_Init(m_hwnd);
					ImGui_ImplOpenGL2_Init();
				}
			}
			

			g_hWnd = m_hwnd;
		}

		g_screen_width = CVarsUpdater::Viewport[2];
		g_screen_height = CVarsUpdater::Viewport[3];

		//static bool once = false;
		//if (!once)
		//{
		//	unsigned char* data = new unsigned char[g_screen_width * g_screen_height * 3];
		//	glPixelStorei(GL_PACK_ALIGNMENT, 1);
		//	glReadPixels(0, 0, g_screen_width, g_screen_height, GL_RGB, GL_UNSIGNED_BYTE, data);
		//	for (uint32_t i = 0; i < g_screen_width * g_screen_height * 3; i += 3)
		//	{
		//		std::cout << "R: " << static_cast<int>(data[i]) << " ";
		//		std::cout << "G: " << static_cast<int>(data[i + 1]) << " ";
		//		std::cout << "B: " << static_cast<int>(data[i + 2]) << "\n";
		//	}

		//	delete[] data;
		//	once = true;
		//}

		for (const auto& Module : CModule::ModuleInstances)
			Module->OnRender();

		if (init_stage == 0)
		{
			ctx = wglCreateContext(hDc);
			wglMakeCurrent(hDc, ctx);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glOrtho(0, CVarsUpdater::Viewport[2], CVarsUpdater::Viewport[3], 0, 1, -1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glClearColor(0, 0, 0, 1);

			init_stage = 1;
		}

		wglMakeCurrent(hDc, ctx);

		if (init_stage == 1)
		{
			ImGui::CreateContext();
			ImGui_ImplWin32_Init(m_hwnd);
			ImGui_ImplOpenGL2_Init();
			auto io = &ImGui::GetIO(); (void)io;
			io->ConfigWindowsMoveFromTitleBarOnly = true;
			m_esp_font = io->Fonts->AddFontDefault();
			ImFontConfig icons_config;
			icons_config.MergeMode = true; icons_config.PixelSnapH = true;
			io->Fonts->AddFontFromMemoryCompressedBase85TTF(base85_compressed_data_fa_solid_900, 24.f , &icons_config, icons_ranges);
			ImGui::SetNextWindowSize({500, 500});
			m_is_imgui_initialized = true;
			init_stage = 3;
		}

		auto& io = ImGui::GetIO();
		io.DisplaySize = { static_cast<float>(g_screen_width), static_cast<float>(g_screen_height) };

		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (m_esp_font_update)
		{
			if (toad::esp::font_path == "Default")
			{
				m_esp_font = io.Fonts->Fonts[0];
			}
			else
			{
				m_esp_font = io.Fonts->AddFontFromFileTTF(toad::esp::font_path.c_str(), 30.f);
				if (!m_esp_font)
				{
					LOGERROR("font was invalid");
					m_esp_font = io.Fonts->Fonts[0];
				}
			
			}

			ImGui_ImplOpenGL2_CreateFontsTexture();

			LOGDEBUG("changing to font: {}", toad::esp::font_path, (void*)m_esp_font);

			m_esp_font_update = false;
		}

		const auto draw = ImGui::GetForegroundDrawList();
		for (const auto& Module : CModule::ModuleInstances)
			Module->OnImGuiRender(draw);

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		wglMakeCurrent(hDc, oCtx);
		return owglSwapBuffers(hDc);
	}

	bool HSwapBuffers::Init()
	{
		return create_hook("opengl32.dll", "wglSwapBuffers", &HSwapBuffers::Hook, reinterpret_cast<LPVOID*>(&owglSwapBuffers));
	}

	void HSwapBuffers::Dispose()
	{
		SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));

		Hook::Dispose();
	}

	void HSwapBuffers::UpdateFont()
	{
		m_esp_font_update = true;
	}

	ImFont* HSwapBuffers::GetFont()
	{
		return m_esp_font;
	}
}
