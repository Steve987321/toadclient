#include "pch.h"
#include "Toad/toad.h"

#include <shlobj_core.h>

#include "logger.h"

namespace toad
{
	Logger::Logger()
	{
#ifdef ALLOCATE_CONSOLE
		m_consoleAllocated = false;
		DWORD alloc_console_res = 0;
		do
		{
			if (!AllocConsole())
			{
				alloc_console_res = GetLastError();
				break;
			}

			freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
			freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
			freopen_s(reinterpret_cast<FILE**>(stderr), "CONERR$", "w", stderr);

			m_hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
			m_consoleAllocated = true;

		} while (false);

#endif 
		// create log file in the documents folder
		m_logFile.open(GetDocumentsFolder() / "Toad.log", std::fstream::out);

		// log the date in the beginning
		logToFile(getDateStr("%Y %d %b \n"));

#ifdef ALLOCATE_CONSOLE
		if (!m_consoleAllocated)
		{
			logToFile(formatStr("[Logger] Failed to allocate console: {}", alloc_console_res));
		}
#endif 
	}

	void Logger::DisposeLogger()
	{
		if (m_logFile.is_open())
			m_logFile.close();

#ifdef ALLOCATE_CONSOLE
		if (!m_consoleAllocated)
			return;

		fclose(stdin);
		fclose(stdout);
		fclose(stderr);

		m_hstdout = nullptr;

		FreeConsole();

		m_consoleAllocated = false;
#endif 

	}

	std::string Logger::getDateStr(const std::string_view format)
	{
		std::ostringstream ss;
		std::string time;

		auto t = std::time(nullptr);
		tm newtime{};

		localtime_s(&newtime, &t);

		ss << std::put_time(&newtime, format.data());
		return ss.str();
	}

	void Logger::logToFile(const std::string_view str)
	{
		m_logFile << str << std::endl;
	}
}
