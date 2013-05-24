#ifndef __CUBIQUITY_LOGGING_H__
#define __CUBIQUITY_LOGGING_H__

#include "PolyVoxCore/Impl/ErrorHandling.h"

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

	// Pull the PolyVox logging functions into Cubiquity
	using PolyVox::logTrace;
	using PolyVox::logDebug;
	using PolyVox::logInfo;
	using PolyVox::logWarning;
	using PolyVox::logError;
	using PolyVox::logFatal;
}

#endif //__CUBIQUITY_LOGGING_H__
