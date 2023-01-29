#ifndef LOGGER_H
#define LOGGER_H

#include "uci.h"
#include <string>


namespace Kojiro{

enum {LOG, INFO, ERROR};

class MyDebugger
{
    private:
        bool m_debug;

    public:
        MyDebugger();
        void setDebug(bool debug);
        void logIt(const std::string message, int type);
		std::string getCurrentDateTime(std::string s);
};

extern MyDebugger logger;
}

#endif