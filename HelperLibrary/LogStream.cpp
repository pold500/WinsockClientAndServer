#include "LogStream.h"

LogStream& GetLogStream()
{
	static LogStream logStream;
	return logStream;
}
