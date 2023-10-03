#include "logging.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <inkview.h>

namespace logging = boost::log;

void taranis::initialize_logging(bool verbose_log) {
  iprofile profile = CreateProfileStruct();
  const auto failed = GetCurrentProfileEx(&profile);
  const auto stem = not failed ? std::string{profile.path} + "/state"
                               : std::string{STATEPATH};
  const auto log_path = stem + "/taranis.log";

  logging::add_common_attributes();

  const auto format =
      (logging::expressions::stream
       << logging::expressions::format_date_time<boost::posix_time::ptime>(
              "TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
       << ": <" << logging::trivial::severity << "> "
       << logging::expressions::smessage);

  auto backend = boost::make_shared<logging::sinks::text_file_backend>(
      logging::keywords::file_name = log_path,
      logging::keywords::rotation_size = 1024 * 1024,
      logging::keywords::format = format);

  typedef logging::sinks::synchronous_sink<logging::sinks::text_file_backend>
      sink_t;
  boost::shared_ptr<sink_t> sink{new sink_t(backend)};

  auto core = logging::core::get();
  core->add_sink(sink);

  if (verbose_log) {
    core->set_filter(logging::trivial::severity >= logging::trivial::debug);

    backend->auto_flush(true);
    // According to boost documentation: This will, of course, degrade
    // logging performance, but in case of an application crash there
    // is a good chance that last log records will not be lost.
  } else {
    core->set_filter(logging::trivial::severity >= logging::trivial::info);
  }
}
