#include "pch.h"
#include "Hooks.h"

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"
#include "Toad/Toad.h"

typedef BOOL(__fastcall* twglSwapBuffers) (_In_ HDC hDc);
twglSwapBuffers owglSwapBuffers = nullptr;

int init = 0;
bool showMenu = false;

HWND hwnd;
HGLRC ctx = nullptr;

BOOL hwglSwapBuffers(HDC hDc)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	toadll::SCREEN_WIDTH = viewport[2];
	toadll::SCREEN_HEIGHT = viewport[3];

	hwnd = WindowFromDC(hDc);
	HGLRC oCtx = wglGetCurrentContext();

	if (init==0)
	{
		ctx = wglCreateContext(hDc);
		wglMakeCurrent(hDc, ctx);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(0, viewport[2], viewport[3], 0, 1, -1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClearColor(0, 0, 0, 1);

		init = 1;
	}

	wglMakeCurrent(hDc, ctx);

	if (init == 1)
	{
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplOpenGL2_Init();
		init = 3;
	}

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSize({400, 300});

	/*ImGui::Begin("nigga client v0.1");

	ImGui::SliderFloat("testing float", &toadll::testingfloat, -1000, 1000);
	if (GetAsyncKeyState(VK_OEM_PLUS))
		toadll::testingfloat += 0.5f;
	else if (GetAsyncKeyState(VK_OEM_MINUS))
		toadll::testingfloat -= 0.5f;

	ImGui::End();*/

	//draw->AddRect({ 25,25 }, { 50, 50 }, IM_COL32_WHITE, 5.f);

	for (const auto& [name, pos] : toadll::esp::playerListMap)
	{
		if (name == nullptr || pos.x < 0) continue;
		const auto draw = ImGui::GetForegroundDrawList();
		//draw->AddCircle({ pos.x, pos.y }, 10.f, IM_COL32_WHITE);
		draw->AddText({pos.x, pos.y }, IM_COL32(255, 255, 255, 255), name);
	}


	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	wglMakeCurrent(hDc, oCtx);
	return owglSwapBuffers(hDc);
}

bool toadll::c_Hooks::init()
{
	if (MH_Initialize() != MH_OK)
		return false;

	optr = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
	log_Debug("wglswapbuffers: %p", optr);

	if (optr == nullptr)
		return false;

	//return MH_CreateHookApi(L"opengl32.dll", "wglSwapBuffers", &hwglSwapBuffers, reinterpret_cast<LPVOID*>(&owglSwapBuffers)) == MH_OK;
	return MH_CreateHook(optr, &hwglSwapBuffers, reinterpret_cast<LPVOID*>(&owglSwapBuffers)) == MH_OK;
}

void toadll::c_Hooks::enable()
{
	enabled = true;
	MH_EnableHook(optr);
}

void toadll::c_Hooks::disable()
{
	enabled = false;
	MH_DisableHook(optr);
}

void toadll::c_Hooks::dispose() const
{
	if (enabled)
		MH_DisableHook(optr);
	MH_Uninitialize();
}
