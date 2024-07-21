#include "pch.h"
#include "Toad/toadll.h"

#include <shlobj_core.h>

#include "logger.h"

namespace toadll
{
	Logger::Logger()
	{
#ifdef ALLOCATE_CONSOLE
		bool failed_to_alloc_console = false;
		DWORD alloc_console_res = 0;
		if (!AllocConsole())
		{
			failed_to_alloc_console = true;
			alloc_console_res = GetLastError();
		}

		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
		freopen_s(reinterpret_cast<FILE**>(stderr), "CONERR$", "w", stderr);

		m_hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
		m_isConsoleClosed = false;
#endif 
		// create log file in the documents folder
		m_logFile.open(getDocumentsFolder() + "\\" + "Toad.log", std::fstream::out);

		// log the date in the beginning
		logToFile(getDateStr("%Y %d %b \n"));

#ifdef ALLOCATE_CONSOLE
		if (failed_to_alloc_console)
		{
			logToFile(formatStr("[Logger] Failed to allocate console: {}", alloc_console_res));
		}
#endif 
	}

	Logger::~Logger()
	{
		DisposeLogger();
	}

	void Logger::DisposeLogger()
	{
		std::lock_guard lock(m_closeMutex);

		if (m_logFile.is_open())
			m_logFile.close();

#ifdef ALLOCATE_CONSOLE

		if (m_isConsoleClosed)
			return;

		fclose(stdin);
		fclose(stdout);
		fclose(stderr);

		CloseHandle(m_hstdout);
		m_hstdout = nullptr;

		FreeConsole();

		m_isConsoleClosed = true;
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

	std::string Logger::getDocumentsFolder()
	{
		CHAR documents[MAX_PATH];
		HRESULT res = SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, documents);
		if (res == S_OK)
		{
			return documents;
		}
		return "";
	}

	void Logger::logToFile(const std::string_view str)
	{
		m_logFile << str << std::endl;
	}
}
