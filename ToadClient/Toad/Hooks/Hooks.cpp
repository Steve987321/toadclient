#include "pch.h"
#include "Hooks.h"
#include "Toad/Toad.h"

typedef BOOL(__fastcall* twglSwapBuffers) (_In_ HDC hDc);
twglSwapBuffers owglSwapBuffers = nullptr;

BOOL hwglSwapBuffers(HDC hDc)
{
	/*GLint viewport[16];
	GLfloat modelview[16];
	GLfloat projection[16];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview );
	glGetFloatv(GL_PROJECTION_MATRIX, projection);

	std::cout << "viewport ";
	for (int i = 0; i < 4; i++)
	{
		std::cout << viewport[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "modelview ";
	for (int i = 0; i < 16; i++)
	{
		std::cout << modelview[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "projection ";
	for (int i = 0; i < 16; i++)
	{
		std::cout << projection[i] << " ";
	}
	std::cout << std::endl;*/

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
