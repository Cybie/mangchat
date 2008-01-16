#include "IRCLog.h"
#include "Log.h"
#include <stdarg.h>

IRCLog::IRCLog()
{
    ircLogfile.open ("irc.log", std::ios::app);
}

IRCLog::~IRCLog()
{
    ircLogfile.close();
}

void IRCLog::WriteLog(const char *what, ...)
{
    va_list ap;
    char tmpoutp[1024];
    va_start(ap, what);
    vsnprintf(tmpoutp, 1024, what, ap );
    va_end(ap);

    ircLogfile << sLog.GetTimestampStr().c_str() << " : " << tmpoutp << "\n";
    ircLogfile.flush();

}
