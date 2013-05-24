#ifndef __CUBIQUITY_LOGGING_H__
#define __CUBIQUITY_LOGGING_H__

#include <string>

namespace Cubiquity
{
	namespace LogLevels
	{
		enum LogLevel
		{
			Disabled,
			Trace,
			Debug,
			Info,
			Warning,
			Error,
			Fatal,
		};
	}
	typedef LogLevels::LogLevel LogLevel;

	void setLogVerbosity(LogLevel minimumLogLevel);

	void logMessage(const std::string& message);
}

#endif //__CUBIQUITY_LOGGING_H__
