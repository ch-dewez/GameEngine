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

#ifdef ENGINE_BUILD
#ifndef NDEBUG
//#define LogEngineTrace(...)		Engine::Log::Log::GetEngineLogger().Trace(__VA_ARGS__)
#define LogEngineDebug(...)		Engine::Log::Log::GetEngineLogger().Debug(__VA_ARGS__)
#define LogEngineInfo(...)		Engine::Log::Log::GetEngineLogger().Info(__VA_ARGS__)
#define LogEngineWarning(...)	        Engine::Log::Log::GetEngineLogger().Warn(__VA_ARGS__)
#define LogEngineError(...)		Engine::Log::Log::GetEngineLogger().Error(__VA_ARGS__)
#else
//#define LogEngineTrace(...)
#define LogEngineDebug(...)
#define LogEngineInfo(...)
#define LogEngineWarning(...)
#define LogEngineError(...)
#endif
#else
#ifndef NDEBUG
//#define LogTrace(...)	Engine::Log::Log::GetGameLogger().Trace(__VA_ARGS__)
#define LogDebug(...)	Engine::Log::Log::GetGameLogger().Debug(__VA_ARGS__)
#define LogInfo(...)	Engine::Log::Log::GetGameLogger().Info(__VA_ARGS__)
#define LogWarning(...)	Engine::Log::Log::GetGameLogger().Warn(__VA_ARGS__)
#define LogError(...)	Engine::Log::Log::GetGameLogger().Error(__VA_ARGS__)
#else
//#define LogTrace(...)
#define LogDebug(...)
#define LogInfo(...)
#define LogWarning(...)
#define LogError(...)
#endif
#endif

}
}
