#include "Logger.hpp"



namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, logger_t)
{
  logger_t lg;



  return lg;
}