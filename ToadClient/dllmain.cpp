// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Toad/toad.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{

    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        toadll::hMod = hModule;
        CloseHandle(CreateThread(nullptr, 0, 
            reinterpret_cast<LPTHREAD_START_ROUTINE>(toadll::init), nullptr, 0, nullptr));
    }
    //if (ul_reason_for_call == DLL_PROCESS_DETACH)
        //CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)toadll::clean_up, 0, 0, 0);

    return TRUE;
}
