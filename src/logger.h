#ifndef LOGGER_H
#define LOGGER_H

// #include "uci.h"


class MyDebugger
{
    private:
        bool m_debug;

    public:
        MyDebugger();
        void setDebug(bool debug);
        void logIt(const char* message);
		std::string getCurrentDateTime(std::string s);
};

MyDebugger::MyDebugger()
{
    m_debug = false;
}

void MyDebugger::setDebug(bool debug)
{
    m_debug = debug;
}

void MyDebugger::logIt(const char* message)
{
    if(m_debug)
    {
	std::string filePath = "./log_"+getCurrentDateTime("date")+".txt";
    std::string now = getCurrentDateTime("now");
	freopen(filePath.c_str(), "w", stderr);
	std::cerr << now << "error message" << std::endl;
	std::clog << now << "log message" << std::endl;
	}
}

inline std::string MyDebugger::getCurrentDateTime( std::string s ){
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);
    if(s=="now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if(s=="date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
};

extern MyDebugger logger;

#endif