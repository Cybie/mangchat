#include "IRCLog.h"

IRCLog::IRCLog()
{
    ircLogfile.open ("irc.log");
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

    ircLogfile << tmpoutp;
    ircLogfile << "\n";

    ircLogfile.flush();

}