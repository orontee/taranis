#pragma once

#include <memory>

#include "fonts.h"
#include "hourlyforecastbox.h"
#include "icons.h"
#include "model.h"
#include "stack.h"
#include "swipe.h"

namespace taranis {
struct ForecastStack : public Stack {
  ForecastStack(int pos_x, int pos_y, int width, int height,
                std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                std::shared_ptr<Fonts> fonts);

  void switch_forecast();

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override;

  bool handle_key_press(int key) override;

  bool handle_key_repeat(int key) override;

  bool handle_key_release(int key) override;

private:
  std::shared_ptr<Model> model;

  std::shared_ptr<HourlyForecastBox> hourly_forecast_box;
  // to call specific methods to reset forecast offset

  bool skip_next_key_release{false};

  SwipeDetector swipe_detector;

  void prepare_child_switch_by_key(int key);

  void request_forecast_switch() const;

  bool handle_possible_swipe(int event_type, int pointer_pos_x,
                             int pointer_pos_y);
};
} // namespace taranis
