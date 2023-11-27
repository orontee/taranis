#pragma once

#include <memory>

#include "fonts.h"
#include "hourlyforecastbox.h"
#include "icons.h"
#include "model.h"
#include "pager.h"
#include "stack.h"
#include "swipe.h"

namespace taranis {
struct ForecastStack : Widget, Paginated {
  ForecastStack(int pos_x, int pos_y, int width, int height,
                std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                std::shared_ptr<Fonts> fonts);

  void switch_forecast();

  std::shared_ptr<HourlyForecastBox> get_hourly_forecast_box() const {
    return this->hourly_forecast_box;
  }

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override;

  bool handle_key_press(int key) override;

  bool handle_key_repeat(int key) override;

  bool handle_key_release(int key) override;

  size_t page_count() const override;

  size_t current_page() const override;

  void do_paint() override;

private:
  static constexpr int vertical_padding{10};
  static constexpr int pager_height{35};

  std::shared_ptr<Model> model;

  std::shared_ptr<DotPager> pager;
  std::shared_ptr<Stack> stack;

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
