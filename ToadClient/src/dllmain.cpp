#include "pch.h"
#include "Toad/toad.h"

#include "Toad/Utils/ipc.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
		Utils::IPC ipc;
        if (!ipc.OpenMappedFile("ToadClientMappingObj"))
			return TRUE;

        ipc.CloseFileMap();

        toad::g_hMod = hModule;
        CloseHandle(CreateThread(nullptr, 0, 
            reinterpret_cast<LPTHREAD_START_ROUTINE>(toad::ToadInit), nullptr, 0, nullptr));
    }
    return TRUE;
}
