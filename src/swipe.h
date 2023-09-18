#pragma once

#include <algorithm>
#include <chrono>
#include <ctime>
#include <inkview.h>
#include <string>

namespace taranis {

enum SwipeType {
  no_swipe = 0,
  left_swipe = -1,
  right_swipe = 1,
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
        const auto delta_x = pointer_pos_x - this->track_start_pos.first;
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

  bool tracking{false};
  std::chrono::time_point<std::chrono::steady_clock> track_start_time;
  std::pair<int, int> track_start_pos;

  void start_tracking(int pointer_pos_x, int pointer_pos_y) {
    this->tracking = true;
    this->track_start_time = std::chrono::steady_clock::now();
    this->track_start_pos = {pointer_pos_x, pointer_pos_y};
  }

  void cancel_tracking() { this->tracking = false; }

  bool is_tracking() const { return this->tracking; }

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
    if (std::abs(pointer_pos_y - this->track_start_pos.second) >
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
    const auto now = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - this->track_start_time);
    return (duration.count() > SwipeDetector::time_threshold);
  }
};
} // namespace taranis
