#pragma once

#include "singleton.h"

#ifdef ERROR
#undef ERROR
#endif 

namespace toadll
{

///
/// Handles the console window and output 
///
class Logger final : public Singleton<Logger>
{
private:
	HANDLE m_hstdout {};

	std::shared_mutex m_mutex {};
	std::shared_mutex m_closeMutex {};
	std::atomic_bool m_isConsoleClosed = false;

	std::ofstream m_logFile{};

public:
	enum class CONSOLE_COLOR : WORD
	{
		GREY = 8,
		WHITE = 15,
		RED = 12,
		GREEN = 10,
		BLUE = 9,
		YELLOW = 14,
		MAGENTA = 13,
	};

	enum class LOG_TYPE : WORD
	{
		DEBUG = static_cast<WORD>(CONSOLE_COLOR::BLUE),
		ERROR = static_cast<WORD>(CONSOLE_COLOR::RED),
		WARNING = static_cast<WORD>(CONSOLE_COLOR::YELLOW),
		EXCEPTION = static_cast<WORD>(CONSOLE_COLOR::MAGENTA)
	};

	std::unordered_map<LOG_TYPE, const char*> logTypeAsStr
	{
	{LOG_TYPE::DEBUG, "DEBUG"},
	{LOG_TYPE::ERROR, "ERROR"},
	{LOG_TYPE::EXCEPTION, "EXCEPTION"},
	{LOG_TYPE::WARNING, "WARNING"},
	};

private:
	static std::string getDateStr(const std::string_view format)
	{
		std::ostringstream ss;
		std::string time;

		auto t = std::time(nullptr);
		tm newtime{};

		localtime_s(&newtime, &t);

		ss << std::put_time(&newtime, format.data());
		return ss.str();
	}

	static std::string getDocumentsFolder()
	{
		CHAR documents[MAX_PATH];
		HRESULT res = SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, documents);
		if (res == S_OK)
		{
			return documents;
		}
		return "";
	}

	template <typename ... Args>
	std::string formatStr(const std::string_view format, Args&& ... args)
	{
		try
		{
			return std::vformat(format, std::make_format_args(args...));
		}
		catch (std::format_error& e)
		{
			LogException("Invalid formatting on string with '{}' | {}", std::string(format).c_str(), e.what());
			return "";
		}
	}

	/// Writes to created log file
	void logToFile(const std::string_view str)
	{
		m_logFile << str << std::endl;
	}

	/// Outputs string to console 
	template<typename ... Args>
	void Print(const std::string_view str, LOG_TYPE log_type)
	{
		std::cout << '[';

		std::cout << logTypeAsStr[log_type];

		std::cout << ']' << ' ';

		std::cout << str << std::endl;
	}

	/// Logs formatted string to console and log file
	///
	///	@param frmt Formatted string that gets formatted with the arguments using '{}'
	///	@param log_type Type of log that affects console colors and beginning message of output
	/// @param args Arguments that fit with the formatted string
	template<typename ... Args>
	void Log(const std::string_view frmt, LOG_TYPE log_type, Args&& ... args)
	{
		std::shared_lock lock(m_mutex);

		auto formattedStr = formatStr(frmt, args...);

		logToFile(getDateStr("[%T]") + ' ' + formattedStr);
		Print(formattedStr, log_type);
	}

public:
	Logger()
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

    ~Logger() override
    {
		DisposeLogger();
	}

public:
	void DisposeLogger()
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

public:
	template <typename ... Args>
	void LogDebug(const char* frmt, Args... args)
	{
		Log(frmt, LOG_TYPE::DEBUG, args...);
	}

	template <typename ... Args>
	void LogWarning(const char* frmt, Args... args)
	{
		Log(frmt, LOG_TYPE::WARNING, args...);
	}

	template <typename ... Args>
	void LogError(const char* frmt, Args... args)
	{
		Log(frmt, LOG_TYPE::ERROR, args...);
	}

	template <typename ... Args>
	void LogException(const char* frmt, Args... args)
	{
		Log(frmt, LOG_TYPE::EXCEPTION, args...);
	}

};

}

#ifdef ENABLE_LOGGING
#define LOGDEBUG(msg, ...) toadll::Logger::GetInstance()->LogDebug(msg, __VA_ARGS__) 
#define LOGERROR(msg, ...) toadll::Logger::GetInstance()->LogError(msg, __VA_ARGS__) 
#define LOGWARN(msg, ...)  toadll::Logger::GetInstance()->LogWarning(msg, __VA_ARGS__)
#else
#define LOGDEBUG(msg, ...)
#define LOGERROR(msg, ...)
#define LOGWARN(msg, ...)
#endif