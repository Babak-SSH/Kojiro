#include "logger.h"
#include <string>

namespace Kojiro{

MyDebugger logger;

MyDebugger::MyDebugger() {
    m_debug = false;
}

void MyDebugger::setDebug(bool debug) {
    m_debug = debug;
}

void MyDebugger::logIt(const std::string message, int type) {
    if(m_debug)
    {
		std::string filePath = "./log_"+getCurrentDateTime("date")+".txt";
    	std::string now = getCurrentDateTime("now");

		if(type==LOG){
			freopen(filePath.c_str(), "a", stderr);
			std::clog << now << "log: " << message << std::endl;
		}	
		else if(type==INFO){
			freopen(filePath.c_str(), "a", stderr);
			std::clog << now << "info: " << message << std::endl;
		}
		else if(type==ERROR){
			freopen(filePath.c_str(), "a", stderr);
			std::cerr << now << "error: " << message << std::endl;
		}
	}
}

inline std::string MyDebugger::getCurrentDateTime( std::string s ) {
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
}