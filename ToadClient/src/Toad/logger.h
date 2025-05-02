#pragma once

#include "toad_defs.h"

#include <mutex>
#include <atomic>
#include <fstream>
#include <wtypes.h>
#include <iostream>
#include <unordered_map>
#include <format>

#ifdef ERROR
#undef ERROR
#endif 

namespace toad
{

///
/// Handles logs and console window
///
class TOAD_API Logger
{
public:
	Logger();

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

	std::ofstream m_logFile{};

	bool m_consoleAllocated = false;
};

}
