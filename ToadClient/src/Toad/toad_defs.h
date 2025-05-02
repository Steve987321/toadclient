#pragma once 

#define ENABLE_LOGGING
#define ALLOCATE_CONSOLE
//#undef ALLOCATE_CONSOLE // so its easier to search for symbol

#ifdef TOAD_LOADER
#define TOAD_API __declspec(dllimport)
#else
#define TOAD_API __declspec(dllexport)
#endif

// used when precision isn't required but the CPU should be saved
#define SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

#ifdef ENABLE_LOGGING
#define LOGDEBUG(msg, ...) toad::g_logger.LogDebug(msg, __VA_ARGS__) 
#define LOGERROR(msg, ...) toad::g_logger.LogError(msg, __VA_ARGS__) 
#define LOGWARN(msg, ...) toad::g_logger.LogWarning(msg, __VA_ARGS__)
#else
#define LOGDEBUG(msg, ...) (void)0
#define LOGERROR(msg, ...) (void)0
#define LOGWARN(msg, ...) (void)0
#endif
