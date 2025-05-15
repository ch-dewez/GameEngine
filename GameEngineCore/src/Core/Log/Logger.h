#include <cstdio>
namespace Engine {
namespace Log {

// based on https://www.youtube.com/watch?v=Vir2EKikbLo
class Logger {
public:
    enum class Priority {
        Debug,
        Info,
        Warning,
        Error
    };

    void setPriority(Priority priority){ m_minimumPriority = priority; };


    template<typename... Args>
    void Debug(const char* message, Args&& ... args){
        Log(Priority::Debug, message, args...);
    }

    template<typename... Args>
    void Info(const char* message, Args&& ... args){
        Log(Priority::Info, message, args...);
    }

    template<typename... Args>
    void Warn(const char* message, Args&& ... args){
        Log(Priority::Warning, message, args...);
    }

    template<typename... Args>
    void Error(const char* message, Args&& ... args){
        Log(Priority::Error, message, args...);
    }

    void setStartingString(const char* string) {m_startingString = string;};

    void resetColor();
private:
    template<typename... Args>
    void Log(Priority priority, const char* message, Args&& ... args){
        if (priority < m_minimumPriority){
            return;
        }
        
        setColor(priority);
        if (m_startingString) {
            std::printf("%s ", m_startingString);
        }
        std::printf("[%s] ", priorityToString(priority));
        std::printf(message, args...);
        std::printf("\n");
        resetColor();
    }

    void setColor(Priority priority);

    const char* priorityToString(Priority priority);

private:
    Priority m_minimumPriority = Priority::Debug;
    const char* m_startingString = nullptr;

    const inline static char* DebugColor = "\033[37m";
    const inline static char* InfoColor = "\033[32m";
    const inline static char* WarningColor = "\033[33m";
    const inline static char* ErrorColor = "\033[31m";
};

}
}


