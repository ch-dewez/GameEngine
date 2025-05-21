#pragma once
#include "Logger.h"

namespace Engine {
namespace Log {

class Log {
public:
    static void init();

    static Engine::Log::Logger& GetEngineLogger() {return *m_engineLogger;};
    static Engine::Log::Logger& GetGameLogger() {return *m_gameLogger;};
private:
    static inline Engine::Log::Logger* m_engineLogger = nullptr;
    static inline Engine::Log::Logger* m_gameLogger = nullptr;
};

#if defined(_MSC_VER)
  #include <intrin.h>
  #define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
  #include <csignal>
  #define DEBUG_BREAK() __builtin_trap() 
#else
  #include <cstdlib>
  #define DEBUG_BREAK() std::abort()
#endif

#ifdef ENGINE_BUILD
#define GetLogger() Engine::Log::Log::GetEngineLogger()
#else
#define GetLogger() Engine::Log::Log::GetGameLogger()
#endif


#ifndef NDEBUG
//#define LogTrace(...)		GetLogger().Trace(__VA_ARGS__)
#define LogDebug(...)		GetLogger().Debug(__VA_ARGS__)
#define LogInfo(...)		GetLogger().Info(__VA_ARGS__)
#define LogWarning(...)	        GetLogger().Warn(__VA_ARGS__)
#define LogError(...)		GetLogger().Error(__VA_ARGS__)
// do stop the program
#define Assert(cond, ...)       do {if (!(cond))  {GetLogger().Error("Assert failed : ", __VA_ARGS__, " in \"", __FILE__, "\" at line ", __LINE__);DEBUG_BREAK();}} while (0)
// doesn't stop the program
#define AssertWarn(cond, ...)   do {if (!(cond))  {GetLogger().Warn("Assert failed : ", __VA_ARGS__, " in \"", __FILE__, "\" at line ", __LINE__);}} while (0)
#else
//#define LogTrace(...)
#define LogDebug(...)
#define LogInfo(...)
#define LogWarning(...)
#define LogError(...)
#define Assert(...)
#define AssertWarn(...)
#endif

}
}
