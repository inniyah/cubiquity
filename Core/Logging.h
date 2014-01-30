#ifndef __CUBIQUITY_LOGGING_H__
#define __CUBIQUITY_LOGGING_H__

#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <ctime>
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
		
		void logTraceMessage(const std::string& /*message*/) {  }
		void logDebugMessage(const std::string& /*message*/) {  }
		void logInfoMessage(const std::string& message) { logToFile("Info   ", message); }
		void logWarningMessage(const std::string& message) { logToFile("Warning", message); }
		void logErrorMessage(const std::string& message) { logToFile("Error  ", message); }
		void logFatalMessage(const std::string& message) { logToFile("Fatal  ", message); }

	private:
		void logToFile(const std::string& type, const std::string& message)
		{
			time_t t = time(0); // get time now
			struct tm * now = localtime( & t );

			// Appending the 'std::endl' forces the stream to be flushed.
			mLogFile << "[" << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << ", " << type << "]: " << message << std::endl; 
		}

		std::ofstream mLogFile;
	};
}

#endif //__CUBIQUITY_LOGGING_H__
