#include <cstdio>
#include <iostream>
#include <sstream>
#include "glm/gtx/io.hpp"

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
    void Debug(Args&& ... args){
        Log(Priority::Debug, args...);
    }

    template<typename... Args>
    void Info(Args&& ... args){
        Log(Priority::Info, args...);
    }

    template<typename... Args>
    void Warn(Args&& ... args){
        Log(Priority::Warning, args...);
    }

    template<typename... Args>
    void Error(Args&& ... args){
        Log(Priority::Error, args...);
    }

    void setStartingString(const char* string) {m_startingString = string;};

    void resetColor();
private:
    template<typename... Args>
    void Log(Priority priority, Args&& ... args){
        if (priority < m_minimumPriority){
            return;
        }

        std::ostringstream oss;
        
        setColor(priority);

        if (m_startingString) {
            oss << m_startingString;
        }

        oss << " [" << priorityToString(priority) << "] ";

        (oss << ... << args);   // fold‐expression: oss << a1 << a2 << …
        oss << '\n';
        std::cout << oss.str();

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


