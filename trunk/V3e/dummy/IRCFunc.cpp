#include "IRCFunc.h"

void TrimCrLf(std::string& sData)
{
	if (sData[sData.size() - 1] == '\x0d' || sData[sData.size() - 1] == '\x0a')
		sData = sData.substr(0, sData.size() - 1);
}

std::string Delink(std::string msg)
{
    std::size_t pos;
    while((pos = msg.find("|Hitem")) != std::string::npos)
    {
        std::size_t find1 = msg.find("|h", pos);
        std::size_t find2 = msg.find("|h", find1 + 2);
        msg.replace(pos, find1 - pos + 2, "\x2");
        msg.replace(msg.find("|h", pos), 2, "\x2");
    }
    while((pos = msg.find("|Henchant")) != std::string::npos)
    {
        std::size_t find1 = msg.find("|h", pos);
        std::size_t find2 = msg.find("|h", find1 + 2);
        msg.replace(pos, find1 - pos + 2, "\x2");
        msg.replace(msg.find("|h", pos), 2, "\x2");
                //msg.replace(find2, 2, "\x2");
    }
    return msg;
}

// This function converts the characters used by the client to identify colour to IRC format.
std::string WoWcol2IRC(std::string msg)
{
    std::size_t pos;
    char IRCCol[9][4] = { "\xF", "\xF", "\x3\x31\x34", "\x3\x30\x33", "\x3\x31\x32", "\x3\x30\x36", "\x3\x30\x37", "\x3\x30\x34", "\x3\x30\x37"};
    char WoWCol[9][12] = { "|r", "|cffffffff", "|cff9d9d9d", "|cff1eff00", "|cff0070dd", "|cffa335ee", "|cffff8000", "|cffe6cc80", "|cffffd000"};
    for (int i=0; i<=8; i++)
    {
        while ((pos = msg.find(WoWCol[i])) != std::string::npos)
        {
            if (i == 0)
                msg.replace(pos, 2, IRCCol[i]);
            else
                msg.replace(pos, 10, IRCCol[i]);
        }
    }
    return msg;
}

// This function converts the characters used by IRC to identify colour to a format the client can understand.
std::string IRCcol2WoW(std::string msg)
{
    std::size_t pos;
    char IRCCol[16][4] = { "\x3\x30", "\x3\x31", "\x3\x32", "\x3\x33", "\x3\x34", "\x3\x35", "\x3\x36", "\x3\x37", "\x3\x38", "\x3\x39", "\x3\x31\x30", "\x3\x31\x31", "\x3\x31\x32", "\x3\x31\x33", "\x3\x31\x34", "\x3\x31\x35"};
    char IRCCol2[10][4] = { "\x3\x30\x30", "\x3\x30\x31", "\x3\x30\x32", "\x3\x30\x33", "\x3\x30\x34", "\x3\x30\x35", "\x3\x30\x36", "\x3\x30\x37", "\x3\x30\x38", "\x3\x30\x39"};
    char WoWcol[16][12] = { "|cffffffff", "|cff000000", "|cff00007f", "|cff009300", "|cffff0000", "|cff7f0000", "|cff9c009c", "|cfffc9300", "|cffffff00", "|cff00fc00", "|cff009393", "|cff00ffff", "|cff0000fc", "|cffff00ff", "|cff7f7f7f", "|cffd2d2d2"};
    for (int i=15; i>=0; i--)
    {
        if (i<10)
        {
            while ((pos = msg.find(IRCCol2[i])) != std::string::npos)
            {
                msg.replace(pos, 3, WoWcol[i]);
            }
            while ((pos = msg.find(IRCCol[i])) != std::string::npos)
            {
                msg.replace(pos, 2, WoWcol[i]);
            }

        }
        else
        {
            while ((pos = msg.find(IRCCol[i])) != std::string::npos)
            {
                msg.replace(pos, 3, WoWcol[i]);
            }
        }
    }

    while ((pos = msg.find("\x3")) != std::string::npos)
    {
        msg.replace(pos, 1, "|r");
    }
    while ((pos = msg.find("\xF")) != std::string::npos)
    {
        msg.replace(pos, 1, "|r");
    }

    return msg;
}

std::string MakeLower(std::string Channel)
{
    std::string tmpchan = Channel;
    std::transform(tmpchan.begin(), tmpchan.end(), tmpchan.begin(), towlower);
    return tmpchan;
}

std::string MakeMsgA(const char *sLine, ... )
{
    va_list ap;
    char tmpoutp[1024];
    va_start(ap, sLine);
    vsnprintf(tmpoutp, 1024, sLine, ap );
    va_end(ap);
    std::string outp = tmpoutp;
    return outp;
}

void MakeMsgW(char *src, const char *sLine, ... )
{
    va_list ap;
    va_start(ap, sLine);
    vsnprintf(src, 1024, sLine, ap );
    va_end(ap);
}

char *Parse(char *src, char *delim, bool bInit, char *bRest)
/* */
{
	if(bRest != NULL)
	{
		src = strtok (NULL, delim);
		while (src != NULL)
		{
			strcat(bRest, src);
			strcat(bRest, delim);
			src = strtok (NULL, delim);
		}
		return bRest;
	}
	if(bInit)
	{
		char *pch = strtok(src, delim);
		return pch;
	}
	else
	{
		src = strtok(NULL, delim);
		if(src == NULL)
			return "";
		else
			return src;
	}
}

#ifdef USE_UTF8
bool ToUTF8(const char *chat)
{
    // extern void error();
    iconv_t cd = iconv_open("char", "UTF-8");
    if (cd != (iconv_t) -1)
    {
        size_t size_orig = strlen(chat);
        size_t size_new = 2040;

        char just_test[2048] = "";
        char *chat_converted = just_test;

        size_t size_converted = iconv(cd, &chat, &size_orig, &chat_converted, &size_new);
        
        if(size_converted != -1)
                    chat = just_test;
        else
        {
            int retval = errno;
            iconv_close(cd);

            switch(retval)
            {
            case EINVAL:
                {
                //Input conversion stopped due to an incomplete character or shift sequence at the end of the input buffer.
                return false;
                }
            case EILSEQ:
                {
                //Input conversion stopped due to an input byte that does not belong to the input codeset
                return false;
                }
            case E2BIG:
                {
                //Input conversion stopped due to lack of space in the output buffer. inbytesleft has the number of bytes to be converted.
                return false;
                }
            }
            }   
        return true;
    }
    else
        return false;
}
#endif
