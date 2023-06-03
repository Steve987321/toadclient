#include "pch.h"
#include "Toad/Toad.h"
#include "wglswapbuffers.h"

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

namespace toadll
{
	typedef BOOL(WINAPI* twglSwapBuffers) (_In_ HDC hDc);

	twglSwapBuffers owglSwapBuffers = nullptr;

	BOOL hook(HDC hDc)
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		toadll::screen_width = viewport[2];
		toadll::screen_height = viewport[3];

		HWND hwnd = WindowFromDC(hDc);
		HGLRC oCtx = wglGetCurrentContext();

		static HGLRC ctx = nullptr;
		static int init_stage = 0;

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
			init_stage = 3;
		}

		auto& io = ImGui::GetIO();
		io.DisplaySize = { static_cast<float>(screen_width), static_cast<float>(screen_height) };

		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//draw->AddRect({ 25,25 }, { 50, 50 }, IM_COL32_WHITE, 5.f);

		const auto draw = ImGui::GetForegroundDrawList();

		for (const auto& Module : moduleInstances)
			Module->OnRender(draw);

		/*for (const auto& [name, pos] : toadll::CEsp::get_instance()->get_playernames_map())
		{
			draw->AddText({ pos.x, pos.y }, IM_COL32(255, 255, 255, 255), name);
		}*/
		/*for (const auto& [name, v4] : toadll::CEsp::get_instance()->get_bounding_box_map())
		{
			draw->AddRect({v4.x, v4.y}, {v4.z, v4.w}, IM_COL32(255, 255, 255, 255));
		}*/


		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

		wglMakeCurrent(hDc, oCtx);
		return owglSwapBuffers(hDc);
	}

	bool c_Swapbuffershook::init()
	{
		return create_hook("opengl32.dll", "wglSwapBuffers", hook, reinterpret_cast<LPVOID*>(&owglSwapBuffers));
	}
}
