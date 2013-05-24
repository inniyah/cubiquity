#include "Logging.h"

#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace Cubiquity
{
	//BOOST_LOG_GLOBAL_LOGGER(my_logger, boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level >)

	BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(my_logger, boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level >)
	{
		boost::log::add_file_log
		(
			boost::log::keywords::file_name = "CubiquityOutput.log",
			boost::log::keywords::format =
			(
				boost::log::expressions::stream
					<< "[" << boost::log::trivial::severity << "]" 
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

	class BoostLogStream : public std::ostream
	{
	private:
		class BoostLogBuf : public std::stringbuf
		{
		private:
			// or whatever you need for your application
			boost::log::trivial::severity_level mSeverityLevel;
		public:
			BoostLogBuf(boost::log::trivial::severity_level severityLevel) : mSeverityLevel(severityLevel) { }
			~BoostLogBuf() {  /*pubsync();*/ }
			int sync()
			{
				boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level >& lg = my_logger::get();

				// PolyVox log messages usually finish with a newline, so they can be easily written directly to cout/cerr.
				// But Boost.Log automatically appends the newline, so we end up with two. Therefore we strip it off first.
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
		// Other constructors could specify filename, etc
		// just remember to pass whatever you need to CLogBuf
		BoostLogStream(boost::log::trivial::severity_level severityLevel) : std::ostream(new BoostLogBuf(severityLevel)) {}
		~BoostLogStream() { delete rdbuf(); }
	};

	BoostLogStream fatalStream(boost::log::trivial::fatal);

	//BOOST_LOG_GLOBAL_LOGGER_INIT(my_logger, boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level >)
	

	void logMessage(const std::string& message)
	{
		boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level >& lg = my_logger::get();
		BOOST_LOG_SEV(lg, boost::log::trivial::info) << message << std::flush;
	}

	class LogConfigurer
	{
	public:
		LogConfigurer()
		{
			PolyVox::setFatalStream(&fatalStream);

			PolyVox::logFatal() << "\n\n" << std::endl;
			PolyVox::logFatal() << "********************************************************************************" << std::endl;
			PolyVox::logFatal() << "***                           Initialising Cubiquity                         ***" << std::endl;
			PolyVox::logFatal() << "********************************************************************************" << std::endl;
		}

		~LogConfigurer()
		{
			PolyVox::setFatalStream(&(std::cerr));
		}
	};

	LogConfigurer gLogConfigurer;
}