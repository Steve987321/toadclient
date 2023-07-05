#include "toad.h"
#include "utils.h"

namespace toad
{
    bool ListProcessModules(DWORD dwPID)
    {
        auto hModuleSnap = INVALID_HANDLE_VALUE;
        MODULEENTRY32 me32;

        // Take a snapshot of all modules in the specified process.
        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
        if (hModuleSnap == INVALID_HANDLE_VALUE)
        {
           // std::cout << (TEXT("CreateToolhelp32Snapshot (of modules)"));
            return false;
        }

        // Set the size of the structure before using it.
        me32.dwSize = sizeof(MODULEENTRY32);

        // Retrieve information about the first module,
        // and exit if unsuccessful
        if (!Module32First(hModuleSnap, &me32))
        {
            std::cout << ("Module32First");  // show cause of failure
            CloseHandle(hModuleSnap);           // clean the snapshot object
            return false;
        }

        // Now walk the module list of the process,
        // and display information about each module
        do
        {
            //_tprintf(TEXT("\n\n     MODULE NAME:     %s"), me32.szModule);
            if (wcscmp(me32.szModule, L"jvm.dll") == 0 || wcscmp(me32.szModule, L"javaw.exe") == 0)
                return true;
           /* _tprintf(TEXT("\n     Executable     = %s"), me32.szExePath);
            _tprintf(TEXT("\n     Process ID     = 0x%08X"), me32.th32ProcessID);
            _tprintf(TEXT("\n     Ref count (g)  = 0x%04X"), me32.GlblcntUsage);
            _tprintf(TEXT("\n     Ref count (p)  = 0x%04X"), me32.ProccntUsage);
            _tprintf(TEXT("\n     Base address   = 0x%08X"), (DWORD)me32.modBaseAddr);
            _tprintf(TEXT("\n     Base size      = %d"), me32.modBaseSize);*/

        } while (Module32Next(hModuleSnap, &me32));

        CloseHandle(hModuleSnap);
        return false;
    }

    static BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam) {
        constexpr DWORD TITLE_SIZE = 1024;
        DWORD PID = 0;

        WCHAR windowTitle[TITLE_SIZE];

        GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

        const int length = ::GetWindowTextLength(hwnd);

        if (IsWindowVisible(hwnd) && length != 0) {
	        //convert to std::string
            auto buf = new char[length + 1];

            char DefChar = ' ';
            WideCharToMultiByte(CP_ACP, 0, windowTitle, -1, buf, length + 1, &DefChar, NULL);

	        auto title = std::string(buf);
            std::transform(title.begin(), title.end(), title.begin(), tolower);

            if (title.find("lunar client") != std::string::npos || title.find("minecraft") != std::string::npos)
            {
                GetWindowThreadProcessId(hwnd, &PID);
                if (ListProcessModules(PID))
					utils::winListVec.emplace_back(title, PID, hwnd);
            }

            delete[] buf;

            return TRUE;
        }
        return TRUE;
    }

    void utils::Fwin_scan()
    {
        while (g_is_running)
        {
            if (!g_is_verified)
            {
                winListVec.clear();
                EnumWindows(enumWindowCallback, 0);
            }
            SLEEP(1000);
        }
    }

}
