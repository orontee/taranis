#pragma once

#include <algorithm>
#include <experimental/optional>
#include <inkview.h>
#include <memory>

#include "model.h"

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {
class LocationHistory {

public:
  static constexpr size_t max_size{5};

  LocationHistory(std::shared_ptr<Model> model) : model{model} {}

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
};
} // namespace taranis
