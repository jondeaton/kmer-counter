#include <boost/log/trivial.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

enum severity_level
{
  normal,
  warning,
  error,
  critical
};

BOOST_LOG_GLOBAL_LOGGER(logger, boost::log::sources::severity_logger_mt< severity_level >)