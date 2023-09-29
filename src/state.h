#pragma once

#include <fstream>
#include <inkview.h>
#include <iomanip>
#include <json/json.h>
#include <memory>
#include <string>

#include "model.h"

namespace taranis {
class ApplicationState {

public:
  ApplicationState(std::shared_ptr<Model> model) : model{model} {
    this->restore();
  }

  ~ApplicationState() { this->dump(); }

  void restore() {
    const auto path = ApplicationState::get_application_state_path();
    std::ifstream input{path, std::ios_base::in};
    if (!input) {
      return;
    }

    Json::Value root;
    input >> root;

    this->restore_location_history(root);
  }

  void dump() {
    const auto path = ApplicationState::get_application_state_path();
    std::ofstream output{path, std::ios_base::out};
    if (!output) {
      return;
    }
    Json::Value root;

    this->dump_location_history(root);

    output << root << std::endl;
    output.close();
  }

private:
  static constexpr char LOCATION_HISTORY_KEY[]{"location_history"};

  std::shared_ptr<Model> model;

  static std::string get_application_state_path() {
    iprofile profile = CreateProfileStruct();
    const auto failed = GetCurrentProfileEx(&profile);
    if (not failed) {
      return std::string{profile.path} + "/state/taranis.json";
    }
    return std::string{STATEPATH} + "/taranis.json";
  }

  void restore_location_history(const Json::Value &root) {
    this->model->location_history.clear();

    for (const auto &value : root[ApplicationState::LOCATION_HISTORY_KEY]) {
      const auto town{value.get("town", "").asString()};
      if (town.empty()) {
        return;
      }
      const HistoryItem item{town, value.get("country", "").asString(),
                             value.get("favorite", false).asBool()};
      this->model->location_history.push_back(item);
    }
  }

  void dump_location_history(Json::Value &root) {
    auto &history_value = root[ApplicationState::LOCATION_HISTORY_KEY];
    const auto &history = this->model->location_history;
    int location_index = 0;
    for (const auto &item : history) {
      history_value[location_index]["town"] = item.location.town;
      history_value[location_index]["country"] = item.location.country;
      history_value[location_index]["favorite"] = item.favorite;

      ++location_index;
    }
  }
};
} // namespace taranis
