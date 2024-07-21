#pragma once

#include "Toad/singleton.h"

#ifdef ERROR
#undef ERROR
#endif 

namespace toadll
{

///
/// Handles logs and console window
///
class Logger final : public Singleton<Logger>
{
public:
	Logger();
	~Logger() override;

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

public:
	/// Returns the directory location to the Documents folder 
	static std::string getDocumentsFolder();

	/// Closes console and log file 
	void DisposeLogger();

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

private:
	/// Returns the current time or date based on the format as a string
	static std::string getDateStr(const std::string_view format);

	/// Writes to created log file
	void logToFile(const std::string_view str);

private:
	/// Formats a string using std::vformat given a string and format arguments and returns it.
	///
	///	brackets '{}' are used for formatting
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
		std::lock_guard lock(m_mutex);
		
		auto formattedStr = formatStr(frmt, args...);

		logToFile(getDateStr("[%T]") + ' ' + formattedStr);

#ifdef ALLOCATE_CONSOLE
		Print(formattedStr, log_type);
#endif
	}

private:
	HANDLE m_hstdout{};

	std::mutex m_mutex{};
	std::mutex m_closeMutex{};

	std::atomic_bool m_isConsoleClosed = true;

	std::ofstream m_logFile{};
};

}

#ifdef ENABLE_LOGGING
#define LOGDEBUG(msg, ...) toadll::Logger::GetInstance()->LogDebug(msg, __VA_ARGS__) 
#define LOGERROR(msg, ...) toadll::Logger::GetInstance()->LogError(msg, __VA_ARGS__) 
#define LOGWARN(msg, ...) toadll::Logger::GetInstance()->LogWarning(msg, __VA_ARGS__)
#else
#define LOGDEBUG(msg, ...) (void)0
#define LOGERROR(msg, ...) (void)0
#define LOGWARN(msg, ...) (void)0
#endif