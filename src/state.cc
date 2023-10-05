#include "state.h"

#include <boost/log/trivial.hpp>
#include <fstream>
#include <inkview.h>
#include <iomanip>
#include <json/writer.h>

constexpr char LOCATION_HISTORY_KEY[]{"location_history"};

namespace taranis {

void ApplicationState::restore() {
  BOOST_LOG_TRIVIAL(debug) << "Restoring application state";

  const auto path = ApplicationState::get_application_state_path();
  std::ifstream input{path, std::ios_base::in};
  if (not input) {
    BOOST_LOG_TRIVIAL(warning)
        << "Failed to open file to restore application state";
    return;
  }

  Json::Value root;
  Json::Reader reader;
  if (not reader.parse(input, root)) {
    BOOST_LOG_TRIVIAL(error) << "Unexpected application state";
    return;
  }
  this->restore_location_history(root);
}

void ApplicationState::dump() {
  BOOST_LOG_TRIVIAL(debug) << "Dumping application state";

  const auto path = ApplicationState::get_application_state_path();
  std::ofstream output{path, std::ios_base::out};
  if (not output) {
    BOOST_LOG_TRIVIAL(warning)
        << "Failed to open file to dump application state";
    return;
  }
  Json::Value root;
  this->dump_location_history(root);

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

void ApplicationState::restore_location_history(const Json::Value &root) {
  this->model->location_history.clear();

  for (const auto &value : root[LOCATION_HISTORY_KEY]) {
    const auto town{value.get("town", "").asString()};
    if (town.empty()) {
      return;
    }
    const HistoryItem item{town, value.get("country", "").asString(),
                           value.get("favorite", false).asBool()};
    this->model->location_history.push_back(item);
  }
}

void ApplicationState::dump_location_history(Json::Value &root) {
  auto &history_value = root[LOCATION_HISTORY_KEY];
  const auto &history = this->model->location_history;
  int location_index = 0;
  for (const auto &item : history) {
    history_value[location_index]["town"] = item.location.town;
    history_value[location_index]["country"] = item.location.country;
    history_value[location_index]["favorite"] = item.favorite;

    ++location_index;
  }
}

} // namespace taranis
