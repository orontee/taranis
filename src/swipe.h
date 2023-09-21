#pragma once

#include <algorithm>
#include <chrono>
#include <ctime>
#include <experimental/optional>
#include <inkview.h>
#include <string>

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {

enum SwipeType {
  no_swipe = 0,
  left_swipe = -1,
  right_swipe = 1,
};

struct TrackData {
  std::chrono::time_point<std::chrono::steady_clock> start_time;
  int start_pos_x;
  int start_pos_y;
};

class SwipeDetector {
public:
  SwipeDetector() {}

  const irect &get_bounding_box() const { return this->bounding_box; }

  void set_bounding_box(const irect &rect) {
    this->bounding_box.x = rect.x;
    this->bounding_box.y = rect.y;
    this->bounding_box.w = rect.w;
    this->bounding_box.h = rect.h;
  }

  SwipeType guess_event_swipe_type(int event_type, int pointer_pos_x,
                                   int pointer_pos_y) {
    if (not(ISPOINTEREVENT(event_type))) {
      return no_swipe;
    }
    auto value = no_swipe;
    if (event_type == EVT_POINTERDOWN) {
      if (this->is_false_positive_track_start(pointer_pos_x, pointer_pos_y)) {
        this->cancel_tracking();
      } else {
        this->start_tracking(pointer_pos_x, pointer_pos_y);
      }
    } else if (event_type == EVT_POINTERMOVE) {
      if (this->is_false_positive_move(pointer_pos_x, pointer_pos_y)) {
        this->cancel_tracking();
      }
    } else if (event_type == EVT_POINTERUP) {
      if (not is_false_positive_track_end(pointer_pos_x, pointer_pos_y)) {
        const auto delta_x = pointer_pos_x - this->track->start_pos_x;
        if (std::abs(delta_x) > SwipeDetector::horizontal_threshold) {
          value = (delta_x > 0) ? right_swipe : left_swipe;
        }
      }
      this->cancel_tracking();
    }
    return value;
  }

private:
  static constexpr double time_threshold{750}; // ms
  static constexpr int horizontal_threshold{300};
  static constexpr int vertical_false_positive_threshold{150};

  irect bounding_box;

  std::optional<TrackData> track;

  void start_tracking(int pointer_pos_x, int pointer_pos_y) {
    this->track = TrackData{std::chrono::steady_clock::now(), pointer_pos_x,
                            pointer_pos_y};
  }

  void cancel_tracking() { this->track = std::experimental::nullopt; }

  bool is_tracking() const { return this->track != std::experimental::nullopt; }

  bool is_false_positive_track_start(int pointer_pos_x,
                                     int pointer_pos_y) const {
    return (this->is_tracking() or
            not IsInRect(pointer_pos_x, pointer_pos_y, &this->bounding_box));
  }

  bool is_false_positive_track_end(int pointer_pos_x, int pointer_pos_y) const {
    if (not this->is_tracking()) {
      return true;
    }
    if (not IsInRect(pointer_pos_x, pointer_pos_y, &this->bounding_box)) {
      return true;
    }
    if (std::abs(pointer_pos_y - this->track->start_pos_y) >
        SwipeDetector::vertical_false_positive_threshold) {
      return true;
    }
    return this->is_pointer_event_late();
  }

  bool is_false_positive_move(int pointer_pos_x, int pointer_pos_y) const {
    return (not this->is_tracking()) or
           not IsInRect(pointer_pos_x, pointer_pos_y, &this->bounding_box) or
           this->is_pointer_event_late();
  }

  bool is_pointer_event_late() const {
    // to be called after a successful call to is_tracking()
    const auto now = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - this->track->start_time);
    return (duration.count() > SwipeDetector::time_threshold);
  }
};
} // namespace taranis
