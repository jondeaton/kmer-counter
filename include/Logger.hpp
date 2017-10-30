#pragma once




enum severity_level
{
  normal,
  warning,
  error,
  critical
};

typedef boost::log::sources::severity_logger_mt<boost::log::trivial::severity> logger_t;

BOOST_LOG_GLOBAL_LOGGER(logger, logger_t)