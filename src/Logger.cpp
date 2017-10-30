#include "GlobalLogger.h"

#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sinks.hpp>
#include <fstream>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;

bool onlyWarnings(const boost::log::attribute_value_set& set)
{
  return set["Severity"].extract<severity_level>() > 0;
}

void severity_and_message(const boost::log::record_view &view, boost::log::formatting_ostream &os)
{
  os << view.attribute_values()["Severity"].extract<severity_level>() << ": " <<
     view.attribute_values()["Message"].extract<std::string>();
}

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, boost::log::sources::severity_logger_mt< severity_level >)
{
boost::log::sources::severity_logger_mt< severity_level > logger;

// add a text sink
typedef sinks::asynchronous_sink<sinks::text_ostream_backend> text_sink;
boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

// add "console" output stream to our sink
boost::shared_ptr<std::ostream> stream{&std::clog, boost::null_deleter{}};
sink->locked_backend()->add_stream(stream);

// specify the format of the log message
sink->set_formatter(&severity_and_message);

// just log messages with severity >= SEVERITY_THRESHOLD are written
sink->set_filter(&onlyWarnings);

// "register" our sink
logging::core::get()->add_sink(sink);

logging::add_common_attributes();

return logger;
}