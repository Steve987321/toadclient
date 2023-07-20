#include "pch.h"
#include "Toad/Toad.h"
#include "logger.h"

namespace toadll
{
	Logger::Logger()
	{
		AllocConsole();

		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
		freopen_s(reinterpret_cast<FILE**>(stderr), "CONERR$", "w", stderr);

		m_hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

		// create log file in the documents folder
		std::string logFileName = "Toad.log";
		m_logFile.open(getDocumentsFolder() + "\\" + logFileName, std::fstream::out);

		// log the date in the beginning
		logToFile(getDateStr("%Y %d %b \n"));
	}

	Logger::~Logger()
	{
		DisposeLogger();
	}

	void Logger::DisposeLogger()
	{
		std::unique_lock lock(m_closeMutex);

		if (m_logFile.is_open())
			m_logFile.close();

		if (m_isConsoleClosed) return;

		fclose(stdin);
		fclose(stdout);
		fclose(stderr);

		CloseHandle(m_hstdout);
		m_hstdout = nullptr;

		FreeConsole();

		m_isConsoleClosed = true;
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
