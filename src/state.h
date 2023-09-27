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
    this->model->location_history.clear();

    Json::Value root;
    input >> root;
    for (const auto &value : root["location_history"]) {
      const auto town{value.get("town", "").asString()};
      if (town.empty()) {
        return;
      }
      const Location location{town, value.get("country", "").asString()};
      this->model->location_history.push_back(location);
    }
  }

  void dump() {
    const auto path = ApplicationState::get_application_state_path();
    std::ofstream output{path, std::ios_base::out};
    if (!output) {
      return;
    }
    Json::Value root;
    auto &history_value = root["location_history"];
    const auto &history = this->model->location_history;
    int location_index = 0;
    for (const auto &location : history) {
      history_value[location_index]["town"] = location.town;
      history_value[location_index]["country"] = location.country;

      ++location_index;
    }
    output << root << std::endl;
    output.close();
  }

private:
  std::shared_ptr<Model> model;

  static std::string get_application_state_path() {
    iprofile profile = CreateProfileStruct();
    const auto failed = GetCurrentProfileEx(&profile);
    if (not failed) {
      return std::string{profile.path} + "/state/taranis.json";
    }
    return std::string{STATEPATH} + "/taranis.json";
  }
};
}
