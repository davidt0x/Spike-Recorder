#ifndef BACKYARDBRAINS_LOG_H
#define BACKYARDBRAINS_LOG_H

#include <cstdio>

namespace BackyardBrains {

class Log {
public:
	Log(const char* log_file_path);
	static void msg(const char *fmt, ...);
	static void warn(const char *fmt, ...);
	static void error(const char *fmt, ...);
	static void fatal(const char *fmt, ...); // aborts program
	static void init(const char * log_file_path);
	static void init();
private:
	static Log *_log;
	FILE *_out;
	const char* _log_file_path;
};

}

#endif
