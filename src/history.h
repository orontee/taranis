#pragma once

#include <algorithm>
#include <experimental/optional>
#include <fstream>
#include <inkview.h>
#include <json/json.h>
#include <memory>

#include "model.h"

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {
class LocationHistory {

public:
  static constexpr size_t max_size{5};

  LocationHistory(std::shared_ptr<Model> model) : model{model} {
    this->deserialize_location_history();
  }

  ~LocationHistory() { this->serialize_location_history(); }

  std::optional<Location> get_location(size_t index) const {
    const auto &history = this->model->location_history;
    auto it = std::begin(history);
    while (it != std::end(history) and index != 0) {
      ++it;
      --index;
    }
    std::optional<Location> result;
    if (index == 0 and it != std::end(history)) {
      result = *it;
    }
    return result;
  }

  void update_history_maybe() {
    auto &history = this->model->location_history;
    auto &current_location = this->model->location;
    auto found = std::find(history.begin(), history.end(), current_location);
    if (found != history.end()) {
      history.erase(found);
    }
    history.push_front(this->model->location);

    if (history.size() > LocationHistory::max_size) {
      history.pop_back();
    }
  }

private:
  std::shared_ptr<Model> model;

  void deserialize_location_history() {
    const auto path = LocationHistory::get_application_state_path();
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

  void serialize_location_history() {
    const auto path = LocationHistory::get_application_state_path();
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

  static std::string get_application_state_path() {
    iprofile profile = CreateProfileStruct();
    const auto failed = GetCurrentProfileEx(&profile);
    if (not failed) {
      return std::string{profile.path} + "/state/taranis.json";
    }
    return std::string{STATEPATH} + "/taranis.json";
  }
};
} // namespace taranis
