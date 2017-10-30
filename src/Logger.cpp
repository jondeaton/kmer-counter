#include "Logger.hpp"

#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, boost::log::sources::severity_logger_mt< severity_level >)
{
  boost::log::sources::severity_logger_mt<severity_level> logger;

  boost::log::add_common_attributes();
  boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

  // log format: [TimeStamp] [Severity Level] Log message
  auto fmtTimeStamp = boost::log::expressions::
  format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S");
  auto fmtSeverity = boost::log::expressions::
  attr<boost::log::trivial::severity_level>("Severity");

  boost::log::formatter logFmt =
    boost::log::expressions::format("[%1%] [%2%] %3%")
    % fmtTimeStamp
    % fmtSeverity
    % boost::log::expressions::smessage;

  // console sink
  auto consoleSink = boost::log::add_console_log(std::clog);
  consoleSink->set_formatter(logFmt);

  return logger;
}