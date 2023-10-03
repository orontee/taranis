#include <inkview.h>
#include <stdexcept>

#include "app.h"
#include "logging.h"

taranis::App app;

int event_handler(int event_type, int param_one, int param_two) {
  try {
    return app.process_event(event_type, param_one, param_two);
  } catch (const std::runtime_error &error) {
    BOOST_LOG_TRIVIAL(error) << "Unhandled runtime error " << error.what();
    throw;
  } catch (const std::logic_error &error) {
    BOOST_LOG_TRIVIAL(error) << "Unhandled logic error " << error.what();
    throw;
  }
}

int main(int argc, char *argv[]) {
  const bool verbose_log_requested =
      (argc == 2 and std::strcmp(argv[1], "--verbose") == 0);
  taranis::initialize_logging(verbose_log_requested);

  InkViewMain(&event_handler);

  return 0;
}
