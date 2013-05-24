#include "Logging.h"

#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <iomanip>
#include <string>

namespace Cubiquity
{
	// This is the main log source, which is created on demand.
	BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(gCubiquityLogger, boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level >)
	{
		boost::log::add_file_log
		(
			boost::log::keywords::file_name = "CubiquityOutput.log",
			boost::log::keywords::format =
			(
				boost::log::expressions::stream
					<< "[" << std::left << std::setw (7) << boost::log::trivial::severity << "]" 
					<< "<" << boost::log::expressions::attr< unsigned int >("ThreadID") << ">: "
					<< boost::log::expressions::smessage
			),
			boost::log::keywords::auto_flush = true,
			boost::log::keywords::open_mode = std::ios_base::app
		);

		boost::log::add_common_attributes();

		boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level > lg;
		return lg;
	}

	// This class provides an implementation of an std::ostream which writes its output to the
	// Boost logging system rather than one of the standard streams. It is used to redirect
	// PolyVox's logging output to Boost. It is based on this code: http://stackoverflow.com/a/4372966
	class BoostLogStream : public std::ostream
	{
	private:
		class BoostLogBuf : public std::stringbuf
		{
		private:
			boost::log::trivial::severity_level mSeverityLevel;
		public:
			// Constructor
			BoostLogBuf(boost::log::trivial::severity_level severityLevel) : mSeverityLevel(severityLevel) { }

			// Destructor doesn't call pubsync() becaus this is causing a crash in Boost.
			// It seems that maybe the Boost.Log source no longer exists at this point?
			~BoostLogBuf() {  /*pubsync();*/ }

			// Sync function writes output to a Boost.Log source.
			int sync()
			{
				boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level >& lg = gCubiquityLogger::get();

				// PolyVox automatically appends an 'std::endl' to messages written to a log stream. But Boost.Log 
				// also automatically appends the newline, so we end up with two. Therefore we strip one off first.
				std::string& message = str();
				if (!message.empty() && message[message.length()-1] == '\n')
				{
					message.erase(message.length()-1);
				}
				BOOST_LOG_SEV(lg, mSeverityLevel) << message;

				str("");
				return 0;
			}
		};

	public:
		BoostLogStream(boost::log::trivial::severity_level severityLevel) : std::ostream(new BoostLogBuf(severityLevel)) {}
		~BoostLogStream() { delete rdbuf(); }
	};

	// std::ostreams which redirect output to Boost.Log
	BoostLogStream traceStream(boost::log::trivial::trace);	
	BoostLogStream debugStream(boost::log::trivial::debug);	
	BoostLogStream infoStream(boost::log::trivial::info);	
	BoostLogStream warningStream(boost::log::trivial::warning);	
	BoostLogStream errorStream(boost::log::trivial::error);	
	BoostLogStream fatalStream(boost::log::trivial::fatal);	

	void setLogVerbosity(LogLevel minimumLogLevel)
	{
		PolyVox::setTraceStream(PolyVox::getNullStream());
		PolyVox::setDebugStream(PolyVox::getNullStream());
		PolyVox::setInfoStream(PolyVox::getNullStream());
		PolyVox::setWarningStream(PolyVox::getNullStream());
		PolyVox::setErrorStream(PolyVox::getNullStream());
		PolyVox::setFatalStream(PolyVox::getNullStream());

		switch(minimumLogLevel)
		{
		case LogLevels::Disabled:
			return;
		case LogLevels::Trace:
			PolyVox::setTraceStream(&traceStream);
		case LogLevels::Debug:
			PolyVox::setDebugStream(&debugStream);
		case LogLevels::Info:
			PolyVox::setInfoStream(&infoStream);
		case LogLevels::Warning:
			PolyVox::setWarningStream(&warningStream);
		case LogLevels::Error:
			PolyVox::setErrorStream(&errorStream);
		case LogLevels::Fatal:
			PolyVox::setFatalStream(&fatalStream);
		}
	}
}