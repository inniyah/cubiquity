#include "FileSystem.h"

#include <Windows.h>

namespace Cubiquity
{
	std::wstring s2ws(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}

	bool directoryExists(const std::string& name)
	{
		std::wstring wideName = s2ws(name);
		DWORD dwAttrib = GetFileAttributes(wideName.c_str());

		return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
			(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}

	bool createDirectory(const std::string& name)
	{
		std::wstring wideName = s2ws(name);
		BOOL result = CreateDirectory(wideName.c_str(), 0);
		return (result == TRUE);
	}
}