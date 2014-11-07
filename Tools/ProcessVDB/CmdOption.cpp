#include "CmdOption.h"

#include <algorithm>

char* getCmdOption(const char ** begin, const char ** end, const std::string & option)
{
    /*char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }*/
    return 0;
}

bool cmdOptionExists(const char** begin, const char** end, const std::string& option)
{
    //return std::find(begin, end, option) != end;
	return 0;
}