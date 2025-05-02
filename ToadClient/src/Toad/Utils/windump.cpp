#include "pch.h"
#include "windump.h"
#include "Toad/Utils/helpers.h"

#include <DbgHelp.h>

namespace Utils
{
	static void CreateMiniDump(struct _EXCEPTION_POINTERS* apExceptionInfo)
	{
		typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE file, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

		HMODULE dbghelp_lib = LoadLibraryA("dbghelp.dll");
		MINIDUMPWRITEDUMP dump = (MINIDUMPWRITEDUMP)GetProcAddress(dbghelp_lib, "MiniDumpWriteDump");

		std::filesystem::path docs_folder = toad::GetDocumentsFolder();

		HANDLE file = ::CreateFileA((docs_folder / "core.dmp").string().c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);

		_MINIDUMP_EXCEPTION_INFORMATION exception_info;
		exception_info.ThreadId = ::GetCurrentThreadId();
		exception_info.ExceptionPointers = apExceptionInfo;
		exception_info.ClientPointers = FALSE;

		dump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpNormal, &exception_info, NULL, NULL);
		::CloseHandle(file);
	}

	LONG WINAPI UnhandledExceptionHandler(struct _EXCEPTION_POINTERS* ep)
	{
		CreateMiniDump(ep);
		return EXCEPTION_CONTINUE_SEARCH;
	}
}