#ifndef __CUBIQUITY_LOGGING_H__
#define __CUBIQUITY_LOGGING_H__

#include "PolyVox/Impl/ErrorHandling.h"

#include <ctime>
#include <fstream>
#include <iomanip>

// Used for getting the path to the user's home folder.
#ifdef __APPLE__
	#include <stdlib.h>
	#include <stdio.h>    
	#include <pwd.h>
	#include <unistd.h>
#endif

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

	class FileLogger : public PolyVox::Logger
	{
	public:
		FileLogger()
			: PolyVox::Logger()
		{

#ifdef __APPLE__
			// From http://stackoverflow.com/a/3734026
			const char *homeDir = getenv("HOME");

			if(!homeDir)
			{
				struct passwd* pwd = getpwuid(getuid());
				if (pwd)
					homeDir = pwd->pw_dir;
			}

			mLogFilePath = homeDir;
			mLogFilePath += "/Library/Logs/Cubiquity.log";
#else
			mLogFilePath = "Cubiquity.log";
#endif

			mLogFile.open(mLogFilePath);

			mLogFile << "********************************************************************************" << std::endl;
			mLogFile << "*                             Initializing Cubiquity                           *" << std::endl;
			mLogFile << "********************************************************************************" << std::endl;
		}

		virtual ~FileLogger()
		{
			mLogFile << "********************************************************************************" << std::endl;
			mLogFile << "*                             Shutting down Cubiquity                          *" << std::endl;
			mLogFile << "********************************************************************************" << std::endl;
			mLogFile << std::endl;
			mLogFile.close();
		}

		std::string getLogFilePath()
		{
			return mLogFilePath;
		}
		
		void logTraceMessage(const std::string& message) { logToFile("Trace  ", message); }
		void logDebugMessage(const std::string& message) { logToFile("Debug  ", message); }
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
			mLogFile << "[" << std::setfill('0') << std::setw(2) << now->tm_hour << ":" << std::setfill('0') << std::setw(2)
				<< now->tm_min << ":" << std::setfill('0') << std::setw(2) << now->tm_sec << ", " << type << "]: " << message << std::endl; 

			// FIXME - For debugging we also log to the console. We should probably create a dedicated console logger for this?
			std::cout << "[" << type << "]: " << message << std::endl;
		}

		std::ofstream mLogFile;
		std::string mLogFilePath;
	};
}

#endif //__CUBIQUITY_LOGGING_H__
