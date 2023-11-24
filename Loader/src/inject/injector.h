#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <TlHelp32.h>
#include <stdio.h>
#include <string>

using f_LoadLibraryA = HINSTANCE(WINAPI*)(const char* lpLibFilename);
using f_GetProcAddress = FARPROC(WINAPI*)(HMODULE hModule, LPCSTR lpProcName);
using f_DLL_ENTRY_POINT = BOOL(WINAPI*)(void* hDll, DWORD dwReason, void* pReserved);

using f_RtlAddFunctionTable = BOOL(WINAPIV*)(PRUNTIME_FUNCTION FunctionTable, DWORD EntryCount, DWORD64 BaseAddress);

struct MANUAL_MAPPING_DATA
{
	f_LoadLibraryA pLoadLibraryA;
	f_GetProcAddress pGetProcAddress;
	f_RtlAddFunctionTable pRtlAddFunctionTable;
	BYTE* pbase;
	HINSTANCE hMod;
	DWORD fdwReasonParam;
	LPVOID reservedParam;
	BOOL SEHSupport;
};

inline std::string inject_status;

bool inject(DWORD pid);