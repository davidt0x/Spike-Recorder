#include "Log.h"
#include "Paths.h"
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cerrno>
#ifdef __APPLE__
#include <syslog.h>
#include <stdarg.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64 

// MSVC defines this in winsock2.h!?
typedef struct timeval {
	long tv_sec;
	long tv_usec;
} timeval;

int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970 
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}
#else
#include <sys/time.h>
#endif


#include <sstream>


namespace BackyardBrains {

Log *Log::_log = 0;
Log::Log() {
    //std::string test  = getConfigPath();
	if(getLoggingPath() == "") {
		_out = stdout;
	} else {
	    fprintf(stderr,"Log file: %s\n", getLoggingPath().c_str());
		_out = fopen(getLoggingPath().c_str(), "w");
		if(_out == 0) {
			fprintf(stderr, "Error opening logging destination:%s\nRedirecting log to stdout.\n", strerror(errno));
			_out = stdout;
		}
	}
}

void Log::init() {
	if(_log == 0)
		_log = new Log();
}

void Log::msg(const char *fmt, ...) {
	init();
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    std::stringstream mystream;

    mystream <<  tp.tv_sec <<" "<<tp.tv_usec / 1000;
	std::string format = "";
	format += mystream.str();
	format += " - ";
	format += fmt;

	format += "\n";
	va_list args;
	va_start(args,fmt);
#ifdef __APPLE__
    char buffer[512];
    vsprintf (buffer,fmt, args);
    //perror (buffer);
    syslog(LOG_ERR, "%s", buffer);
#else
	vfprintf(_log->_out, format.c_str(), args);
#endif



	va_end(args);
}

void Log::warn(const char *fmt, ...) {
	init();

	std::string format = "Warning: ";
	format += fmt;
	format += "\n";
	va_list args;
	va_start(args,fmt);

    #ifdef __APPLE__
        char buffer[512];
        vsprintf (buffer,fmt, args);
        syslog(LOG_WARNING, "%s", buffer);
    #else
        vfprintf(_log->_out, format.c_str(), args);
    #endif
	va_end(args);
}

void Log::error(const char *fmt, ...) {
	init();

	std::string format = "Error: ";
	format += fmt;
	format += "\n";
	va_list args;
	va_start(args,fmt);
#ifdef __APPLE__
    char buffer[512];
    vsprintf (buffer,fmt, args);
    syslog(LOG_ERR, "%s", buffer);
#else
    vfprintf(_log->_out, format.c_str(), args);
#endif
	va_end(args);
}

void Log::fatal(const char *fmt, ...) {
	init();

	std::string format = "FATAL: ";
	format += fmt;
	format += "\n";
	va_list args;
	va_start(args,fmt);
#ifdef __APPLE__
    char buffer[512];
    vsprintf (buffer,fmt, args);
    syslog(LOG_CRIT, "%s", buffer);
#else
    vfprintf(_log->_out, format.c_str(), args);
#endif
	va_end(args);

	abort();
}

}
