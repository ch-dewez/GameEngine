#include "Log.h"


namespace Engine {
namespace Log {

void Log::init(){
#ifndef NDEBUG
    if (m_engineLogger){
        delete m_engineLogger;
    }
    if (m_gameLogger){
        delete m_gameLogger;
    }

    m_engineLogger = new Logger();
    m_engineLogger->setStartingString("[ENGINE]");
    m_gameLogger = new Logger();
    m_gameLogger->setStartingString("[GAME]");
#endif
};


}
}
