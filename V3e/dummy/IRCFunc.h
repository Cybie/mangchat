#ifndef _MC_FUNC_H
#define _MC_FUNC_H

#include "common.h"

void TrimCrLf(std::string& sData);
std::string Delink(std::string msg);
std::string WoWcol2IRC(std::string msg);
std::string IRCcol2WoW(std::string msg);
std::string MakeLower(std::string Channel);

std::string MakeMsgA(const char *sLine, ... );
void MakeMsgW(char *src, const char *sLine, ... );

char *Parse(char *src, char *delim = " ", bool bInit = false, char *bRest = NULL);

bool ToUTF8(const char *chat);

#endif
