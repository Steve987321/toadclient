#include "toad.h"
#include "utils.h"

#include "imgui/imgui.h"

namespace toad
{
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
	            utils::winList.emplace_back(title, PID, hwnd);
            }

            delete[] buf;

            return TRUE;
        }
        return TRUE;
    }

    void utils::fwin_scan_thread()
    {
        while (toad::is_running)
        {
	         if (!toad::is_verified)
			 {
                 utils::winList.clear();
                 EnumWindows(enumWindowCallback, 0);

	             SLOW_SLEEP(1000);
	         }
			
        }
    }

}
