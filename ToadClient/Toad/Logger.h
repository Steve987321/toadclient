#pragma once
#include "singleton.h"

namespace toadll
{

///
/// Handles the console window and output 
///
class Logger final : public Singleton<Logger>
{
private:
	HANDLE m_hconsole {};
	std::shared_mutex m_mutex {};
	std::shared_mutex m_closeMutex {};
	std::atomic_bool m_isConsoleClosed = false;
	FILE* m_fcout = nullptr;
	FILE* m_fcerr = nullptr;

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

private:
	std::unordered_map<LOG_TYPE, const char*> logTypeAsStr
	{
	{LOG_TYPE::DEBUG, "DEBUG"},
	{LOG_TYPE::ERROR, "ERROR"},
	{LOG_TYPE::EXCEPTION, "EXCEPTION"},
	{LOG_TYPE::WARNING, "WARNING"},
	};

private:
	template <typename ... Args>
	std::string formatStr(const char* format, Args ... args)
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

	template<typename ... Args>
	void Print(const char* frmt, LOG_TYPE type, Args... args)
	{
		std::shared_lock lock(m_mutex);
		std::string formatted_str = formatStr(frmt, args...);

		std::cout << '[';

		SetConsoleTextAttribute(m_hconsole, static_cast<WORD>(type));
		std::cout << logTypeAsStr[type];
		SetConsoleTextAttribute(m_hconsole, static_cast<WORD>(CONSOLE_COLOR::WHITE));

		std::cout << ']' << ' ';

		bool isTypeDbg = type == LOG_TYPE::DEBUG;

		SetConsoleTextAttribute(m_hconsole, static_cast<WORD>(isTypeDbg ? CONSOLE_COLOR::GREY : CONSOLE_COLOR::WHITE));

		std::cout << formatted_str << std::endl;

		if (isTypeDbg)
		{
			SetConsoleTextAttribute(m_hconsole, static_cast<WORD>(CONSOLE_COLOR::WHITE));
		}
	}

public:
	Logger()
	{
		AllocConsole();
		AttachConsole(ATTACH_PARENT_PROCESS);
		SetConsoleTitle(L"Console");
		freopen_s(&m_fcout, "CONOUT$", "w", stdout);
		freopen_s(&m_fcerr, "CONOUT$", "w", stderr);
		m_hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}
    ~Logger() override
    {
		DisposeConsole();
	}

public:
	void DisposeConsole()
	{
		std::unique_lock lock(m_closeMutex);

		if (m_isConsoleClosed) return;

		fclose(m_fcout);
		fclose(m_fcerr);
		CloseHandle(m_hconsole);
		FreeConsole();
		m_isConsoleClosed = true;
	}
public:
	template <typename ... Args>
	void Log(const char* frmt, Args... args)
	{
		Print(frmt, LOG_TYPE::DEBUG, args...);
	}

	template <typename ... Args>
	void LogWarning(const char* frmt, Args... args)
	{
		Print(frmt, LOG_TYPE::WARNING, args...);
	}

	template <typename ... Args>
	void LogError(const char* frmt, Args... args)
	{
		Print(frmt, LOG_TYPE::ERROR, args...);
	}

	template <typename ... Args>
	void LogException(const char* frmt, Args... args)
	{
		Print(frmt, LOG_TYPE::EXCEPTION, args...);
	}

};

}

#ifdef ENABLE_LOGGING
#define LOGDEBUG(msg, ...) toadll::Logger::GetInstance()->Log(msg, __VA_ARGS__) 
#define LOGERROR(msg, ...) toadll::Logger::GetInstance()->LogError(msg, __VA_ARGS__) 
#define LOGWARN(msg, ...)  toadll::Logger::GetInstance()->LogWarning(msg, __VA_ARGS__)
#else
#define LOGDEBUG(msg, ...)
#define LOGERROR(msg, ...)
#define LOGWARN(msg, ...)
#endif