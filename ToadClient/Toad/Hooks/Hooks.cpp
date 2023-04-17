#include "pch.h"
#include "Hooks.h"
#include "Toad/Toad.h"

bool toadll::c_Hook::create_hook(const char* moduleName, const char* procName, void* detour, void** original)
{
	if (!mh_initialized)
		if (MH_Initialize() != MH_OK)
			return false;
	mh_initialized = true;

	optr = GetProcAddress(GetModuleHandleA(moduleName), procName);
	log_Debug("optr: %p", optr);

	if (optr == nullptr)
		return false;

	//return MH_CreateHookApi(L"opengl32.dll", "wglSwapBuffers", &hwglSwapBuffers, reinterpret_cast<LPVOID*>(&owglSwapBuffers)) == MH_OK;
	//return MH_CreateHook(optr, &hwglSwapBuffers, reinterpret_cast<LPVOID*>(&owglSwapBuffers)) == MH_OK;
	return MH_CreateHook(optr, detour, original) == MH_OK;
	//return MH_CreateHookApi(reinterpret_cast<LPCWSTR>(moduleName), procName, detour, &original) == MH_OK;
}

bool toadll::c_Hook::is_null() const
{
	return optr == nullptr;
}

void toadll::c_Hook::enable()
{
	enabled = true;
	MH_EnableHook(optr);
}

void toadll::c_Hook::disable()
{
	enabled = false;
	MH_DisableHook(optr);
}

void toadll::c_Hook::dispose() const
{
	if (enabled)
		MH_DisableHook(optr);
}
