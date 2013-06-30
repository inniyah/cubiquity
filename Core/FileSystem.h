#ifndef __CUBIQUITY_FILE_SYSTEM_H__
#define __CUBIQUITY_FILE_SYSTEM_H__

#include <string>

namespace Cubiquity
{
	bool directoryExists(const std::string& name);
	bool createDirectory(const std::string& name);
}

#endif //__CUBIQUITY_FILE_SYSTEM_H__
