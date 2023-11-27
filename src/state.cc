#include "state.h"

#include <boost/log/trivial.hpp>
#include <fstream>
#include <inkview.h>
#include <iomanip>
#include <json/writer.h>

#include "convert.h"

namespace taranis {

constexpr char MODEL_KEY[]{"model"};

void ApplicationState::restore() {
  BOOST_LOG_TRIVIAL(debug) << "Restoring application state";

  const auto path = ApplicationState::get_application_state_path();
  std::ifstream input{path};
  if (not input) {
    BOOST_LOG_TRIVIAL(warning)
        << "Failed to open file to restore application state";
    return;
  }

  Json::Value root;
  Json::Reader reader;
  if (not reader.parse(input, root)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to read stored application state";
    BOOST_LOG_TRIVIAL(error) << reader.getFormatedErrorMessages();
    return;
  }
  this->restore_model(root);
}

void ApplicationState::dump() {
  BOOST_LOG_TRIVIAL(debug) << "Dumping application state";

  const auto path = ApplicationState::get_application_state_path();
  std::ofstream output{path};
  if (not output) {
    BOOST_LOG_TRIVIAL(warning)
        << "Failed to open file to dump application state";
    return;
  }
  Json::Value root;
  this->dump_model(root);

  output << root << std::endl;
  if (not output) {
    BOOST_LOG_TRIVIAL(error) << "Failed to stream application state";
  }
  output.close();
}

std::string ApplicationState::get_application_state_path() {
  iprofile profile = CreateProfileStruct();
  const auto failed = GetCurrentProfileEx(&profile);
  const auto stem = not failed ? std::string{profile.path} + "/state"
                               : std::string{STATEPATH};
  const auto application_state_path = stem + "/taranis.json";

  BOOST_LOG_TRIVIAL(info) << "Application state path "
                          << application_state_path;

  return application_state_path;
}

void ApplicationState::restore_model(const Json::Value &root) {
  const auto &model_value = root[MODEL_KEY];
  update_from_json(*this->model, model_value);

  BOOST_LOG_TRIVIAL(info) << "Model restored from application state";
}

void ApplicationState::dump_model(Json::Value &root) {
  auto &model_value = root[MODEL_KEY];
  model_value = to_json(*this->model);

  BOOST_LOG_TRIVIAL(info) << "Model dumped to application state";
}

} // namespace taranis
