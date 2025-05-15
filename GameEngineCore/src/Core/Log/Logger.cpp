#include "Logger.h"
#include <cstdio>
#include <ctime>

namespace Engine {
namespace Log {

const char* Logger::priorityToString(Priority priority){
    switch (priority) {
        case Priority::Debug:{
            return "Debug";
        }
        case Priority::Info: {
            return "Info";
        }
        case Priority::Warning: {
            return "Warning";
        }
        case Priority::Error: {
            return "Error";
        }
    }
}

void Logger::setColor(Priority priority){
    switch (priority) {
        case Priority::Debug:{
            std::printf("%s", DebugColor);
            break;
        }
        case Priority::Info: {
            std::printf("%s", InfoColor);
            break;
        }
        case Priority::Warning: {
            std::printf("%s", WarningColor);
            break;
        }
        case Priority::Error: {
            std::printf("%s", ErrorColor);
            break;
        }
    }
}

void Logger::resetColor(){
    std::printf("\33[0m");
};

}
}


