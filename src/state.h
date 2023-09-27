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
    this->restore_favorite_locations(root);
  }

  void dump() {
    const auto path = ApplicationState::get_application_state_path();
    std::ofstream output{path, std::ios_base::out};
    if (!output) {
      return;
    }
    Json::Value root;

    this->dump_location_history(root);
    this->dump_favorite_locations(root);

    output << root << std::endl;
    output.close();
  }

private:
  static constexpr char LOCATION_HISTORY_KEY[]{"location_history"};
  static constexpr char FAVORITE_LOCATIONS_KEY[]{"favorite_locations"};

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
      const Location location{town, value.get("country", "").asString()};
      this->model->location_history.push_back(location);
    }
  }

  void dump_location_history(Json::Value &root) {
    auto &history_value = root[ApplicationState::LOCATION_HISTORY_KEY];
    const auto &history = this->model->location_history;
    int location_index = 0;
    for (const auto &location : history) {
      history_value[location_index]["town"] = location.town;
      history_value[location_index]["country"] = location.country;

      ++location_index;
    }
  }

  void restore_favorite_locations(const Json::Value &root) {
    this->model->favorite_locations.clear();

    for (const auto &value : root[ApplicationState::FAVORITE_LOCATIONS_KEY]) {
      const auto town{value.get("town", "").asString()};
      if (town.empty()) {
        return;
      }
      const Location location{town, value.get("country", "").asString()};
      this->model->favorite_locations.push_back(location);
    }
  }

  void dump_favorite_locations(Json::Value &root) {
    auto &favorite_locations_value =
        root[ApplicationState::FAVORITE_LOCATIONS_KEY];
    const auto &favorites = this->model->favorite_locations;
    int favorite_index = 0;
    for (const auto &location : favorites) {
      favorite_locations_value[favorite_index]["town"] = location.town;
      favorite_locations_value[favorite_index]["country"] = location.country;

      ++favorite_index;
    }
  }
};
} // namespace taranis
