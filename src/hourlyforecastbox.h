#pragma once

#include <memory>

#include "fonts.h"
#include "icons.h"
#include "model.h"
#include "widget.h"

namespace taranis {

class HourlyForecastBox : public Widget {
public:
  HourlyForecastBox(int pos_x, int pos_y, int width, int height,
                    std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                    std::shared_ptr<Fonts> fonts);

  void show() override;

  void increase_forecast_offset();

  void decrease_forecast_offset();

private:
  static constexpr int vertical_padding{25};
  static constexpr int icon_size{100};

  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  size_t visible_bars;
  int bar_width;

  int frame_start_x;
  int frame_start_y;

  int time_y;
  int icon_y;
  int temperature_y;
  int wind_speed_y;
  int humidity_y;

  int bars_height;
  int curve_y_offset;
  int curve_height;

  size_t forecast_offset{0};

  bool handle_key_press(int key) override;

  bool handle_key_repeat(int key) override;

  bool handle_key_release(int key) override;

  void draw_and_update();

  void draw_frame_and_values() const;

  void draw_temperature_curve() const;

  void draw_precipitation_histogram() const;

  void request_change_display_forecast_display();
};

} // namespace taranis
