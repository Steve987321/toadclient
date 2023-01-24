#include "pch.h"
#include "Hooks.h"
#include "Toad/Toad.h"

typedef BOOL(__fastcall* twglSwapBuffers) (_In_ HDC hDc);
twglSwapBuffers owglSwapBuffers = nullptr;

BOOL hwglSwapBuffers(HDC hDc)
{
	std::cout << "we are in \n";
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	std::cout << viewport[0] << " " << viewport[1] << " " << viewport[2] << " " << viewport[3] << std::endl;
	return owglSwapBuffers(hDc);
}

bool c_Hooks::init()
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

void c_Hooks::enable()
{
	enabled = true;
	MH_EnableHook(optr);
}

void c_Hooks::disable()
{
	enabled = false;
	MH_DisableHook(optr);
}

void c_Hooks::dispose() const
{
	if (enabled)
		MH_DisableHook(optr);
	MH_Uninitialize();
}
