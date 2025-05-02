#include "pch.h"
#include "Toad/toad.h"
#include "injector.h"

namespace Injector
{

static std::function<void(const std::string&)> status_callback{};

bool Inject(DWORD pid, const std::filesystem::path& dll_path, std::function<void(const std::string&)> callback)
{
	status_callback = callback;
	std::cout << "injecting " << dll_path << std::endl;
	if (!std::filesystem::exists(dll_path))
	{
		status_callback(std::format("{} dll doesn't exist", dll_path.string()));
		return false;
	}

	HANDLE proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!proc_handle)
	{
		status_callback(std::format("OpenProcess returned invalid handle. GetLastError(): {}", GetLastError()));
		return false;
	}

	// Allocate for dll path string in process

	// + 1 for string terminator
	size_t alloc_size = dll_path.string().length() + 1;
	LPVOID proc_dll_path = VirtualAllocEx(proc_handle, 0, alloc_size, MEM_COMMIT, PAGE_READWRITE);
	if (!proc_dll_path)
	{
		CloseHandle(proc_handle);

		status_callback(std::format("Failed to allocate {} to process. GetLastError(): {}", alloc_size, GetLastError()));
		return false;
	}

	// Write the path to the address of the memory we just allocated in the target process
	if (!WriteProcessMemory(proc_handle, proc_dll_path, (LPVOID)dll_path.string().c_str(), dll_path.string().length() + 1, 0))
	{
		CloseHandle(proc_handle);
		status_callback("Failed to write path");
		return false;
	}

	// Create a loader thread to call loadlibrary with toad dll
	HANDLE hLoadThread = CreateRemoteThread(proc_handle, 0, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"), proc_dll_path, 0, 0);
	
	// Wait for it to finish
	WaitForSingleObject(hLoadThread, INFINITE); 

	CloseHandle(hLoadThread);

	if (!VirtualFreeEx(proc_handle, proc_dll_path, 0, MEM_RELEASE))
		status_callback("Failed to release memory for dll path");

	CloseHandle(proc_handle);

	return true;
}

}
