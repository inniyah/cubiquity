#include "Logging.h"

#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace Cubiquity
{
	BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(my_logger, boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level >)
	{
		boost::log::add_file_log
		(
			boost::log::keywords::file_name = "Cubiquity.log",
			boost::log::keywords::format =
			(
				boost::log::expressions::stream
					<< "[" << boost::log::trivial::severity << "]" 
					<< "<" << boost::log::expressions::attr< unsigned int >("ThreadID") << ">: "
					<< boost::log::expressions::smessage
			)
		);

		boost::log::add_common_attributes();

		boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level > lg;
		return lg;
	}

	void logMessage(const std::string& message)
	{
		boost::log::sources::severity_logger_mt< boost::log::trivial::severity_level >& lg = my_logger::get();
		BOOST_LOG_SEV(lg, boost::log::trivial::info) << message;
	}
}