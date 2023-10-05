#pragma once

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <experimental/optional>
#include <fstream>
#include <inkview.h>
#include <memory>

#include "model.h"

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {
class LocationHistoryProxy {

public:
  static constexpr size_t max_size{5};

  LocationHistoryProxy(std::shared_ptr<Model> model) : model{model} {}

  ~LocationHistoryProxy() {}

  std::optional<Location> get_location(size_t index) const {
    const auto &history = this->model->location_history;
    auto it = std::begin(history);
    while (it != std::end(history) and index != 0) {
      ++it;
      --index;
    }
    std::optional<Location> result;
    if (index == 0 and it != std::end(history)) {
      result = it->location;
    }
    return result;
  }

  void update_history_maybe() {
    BOOST_LOG_TRIVIAL(debug) << "Updating history";

    auto &current_location = this->model->location;
    auto &history = this->model->location_history;
    const auto found = std::find_if(history.begin(), history.end(),
                                    [current_location](const auto &item) {
                                      return item.location == current_location;
                                    });
    const bool found_favorite = (found != history.end() and found->favorite);
    if (found != history.end()) {
      if (found == history.begin()) {
        BOOST_LOG_TRIVIAL(debug) << "History already up-to-date";
        return;
      }
      BOOST_LOG_TRIVIAL(debug) << "Removing history item to avoid duplicates";

      history.erase(found);
    }
    if (history.size() == LocationHistoryProxy::max_size) {
      auto last_not_favorite =
          std::find_if(history.rbegin(), history.rend(),
                       [](const auto &item) { return not item.favorite; });
      if (last_not_favorite != history.rend()) {
        BOOST_LOG_TRIVIAL(debug)
            << "Removing last not favorite history item since history is full";

        history.erase(std::prev(last_not_favorite.base()));
      }
    }
    if (history.size() < LocationHistoryProxy::max_size) {
      BOOST_LOG_TRIVIAL(debug) << "Adding new item in history";

      history.push_front(HistoryItem{this->model->location, found_favorite});
    }
  }

  bool is_current_location_favorite() const {
    auto &current_location = this->model->location;
    auto &history = this->model->location_history;
    const auto found = std::find_if(history.begin(), history.end(),
                                    [current_location](const auto &item) {
                                      return item.location == current_location;
                                    });
    return found != history.end() and found->favorite;
  }

  void toggle_current_location_favorite() {
    auto &current_location = this->model->location;
    auto &history = this->model->location_history;
    auto found = std::find_if(history.begin(), history.end(),
                              [current_location](const auto &item) {
                                return item.location == current_location;
                              });
    if (found == history.end()) {
      return;
    }
    found->favorite = (not found->favorite);
  }

  bool can_add_favorite() {
    auto &history = this->model->location_history;
    const auto found =
        std::find_if(history.begin(), history.end(),
                     [](const auto &item) { return not item.favorite; });
    return found != history.end();
  }

private:
  std::shared_ptr<Model> model;
};
} // namespace taranis
