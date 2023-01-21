#pragma once

#include <Windows.h>
#include <shared_mutex>
#include <iomanip>
#include <sstream>

namespace toadll
{

class c_Logger
{
private:
	std::shared_mutex m_mutex;
	HANDLE m_hOutput;
	FILE* m_f = nullptr;

private:
	static std::string get_time()
	{
		std::ostringstream ss;
		std::string time;

		auto t = std::time(0);
		tm newtime{};
		localtime_s(&newtime, &t);

		ss << std::put_time(&newtime, "%H:%M:%S");
		return ss.str();
	}

public:
	inline enum class log_type
	{
		LOK = 10,	    // green
		LERROR = 12,	// red
		LDEBUG = 9,		// blue
		LWARNING = 14	// yellow
	};

	c_Logger()
	{
		AllocConsole();
		freopen_s(&m_f, "CONOUT$", "w", stdout);
		m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	~c_Logger()
	{
		dispose_console();
	}

	void dispose_console() const
	{
		FreeConsole();
		fclose(m_f);
	}

	template <typename ... args>
	void Print(log_type type, args... Args)
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);

		SetConsoleTextAttribute(m_hOutput, (WORD)type);

		const char* logtypec;
		std::string time = get_time();

		switch (type)
		{
		case log_type::LDEBUG:
			logtypec = "[#]";
			break;
		case log_type::LERROR:
			logtypec = "[-]";
			break;
		case log_type::LOK:
			logtypec = "[+]";
			break;
		case log_type::LWARNING:
			logtypec = "[!]";
			break;
		default:
			logtypec = "[?]";
			break;

		}

		std::cout << logtypec << ' ';

		SetConsoleTextAttribute(m_hOutput, 8); // white

		std::cout << time << ' ';

		if (type != log_type::LDEBUG) SetConsoleTextAttribute(m_hOutput, 15);

		printf(Args..., Args...);
		std::cout << std::endl;
	}

};

inline std::unique_ptr<c_Logger> p_Log;

}


#define log_Ok(msg, ...) sim::p_Log->Print(sim::Logger::log_type::LOK, msg, __VA_ARGS__); 
#define log_Debug(msg, ...) sim::p_Log->Print(sim::Logger::log_type::LDEBUG, msg, __VA_ARGS__); 
#define log_Error(msg, ...) sim::p_Log->Print(sim::Logger::log_type::LERROR, msg, __VA_ARGS__); 
#define log_Warn(msg, ...) sim::p_Log->Print(sim::Logger::log_type::LWARNING, msg, __VA_ARGS__); 