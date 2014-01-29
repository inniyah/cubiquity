#ifndef __CUBIQUITY_LOGGING_H__
#define __CUBIQUITY_LOGGING_H__

#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <fstream>

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

	// Pull the PolyVox logging functions into Cubiquity
	using PolyVox::logTrace;
	using PolyVox::logDebug;
	using PolyVox::logInfo;
	using PolyVox::logWarning;
	using PolyVox::logError;
	using PolyVox::logFatal;

	class FileLogger : public PolyVox::Logger
	{
	public:
		FileLogger(const std::string& pathToLogFile)
			: PolyVox::Logger()
		{
			mLogFile.open(pathToLogFile);
		}

		virtual ~FileLogger()
		{
			mLogFile.close();
		}

		std::ostream& getTraceStream() { return getNullStream(); }
		std::ostream& getDebugStream() { return getNullStream(); }
		std::ostream& getInfoStream() { return mLogFile; }
		std::ostream& getWarningStream() { return mLogFile; }
		std::ostream& getErrorStream() { return mLogFile; }
		std::ostream& getFatalStream() { return mLogFile; }

	private:
		std::ofstream mLogFile;
	};
}

#endif //__CUBIQUITY_LOGGING_H__
