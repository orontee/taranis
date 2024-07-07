#pragma once

#include <map>
#include <memory>
#include <tuple>

#include "fonts.h"
#include "icons.h"
#include "model.h"
#include "pager.h"
#include "widget.h"

namespace taranis {

class HourlyForecastBox : public Widget, public Paginated {
public:
  HourlyForecastBox(int pos_x, int pos_y, int width, int height,
                    std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                    std::shared_ptr<Fonts> fonts);

  size_t get_forecast_offset() const;

  size_t get_min_forecast_offset() const;

  size_t get_max_forecast_offset() const;

  void set_min_forecast_offset();

  void set_max_forecast_offset();

  void increase_forecast_offset();

  void decrease_forecast_offset();

  void do_paint() override;

  bool handle_key_press(int key) override;

  bool handle_key_release(int key) override;

  size_t page_count() const override;

  size_t current_page() const override;

private:
  static constexpr size_t visible_bars{8};
  static constexpr int vertical_padding{25};
  static constexpr int icon_size{100};
  static constexpr int wind_direction_icon_size{50};

  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  std::map<int, const ibitmap *> rotated_direction_icons;

  const ibitmap *const direction_icon;

  int bar_width;

  int frame_start_x;
  int frame_start_y;

  int date_labels_start_y;
  int time_y;
  int weather_icon_y;
  int temperature_y;
  int wind_direction_icon_y;
  int wind_speed_y;
  int wind_gust_y;

  int bars_height;
  int curve_y_offset;
  int curve_height;

  size_t forecast_offset{0};

  std::tuple<std::string, int>
  get_date_label_properties(size_t bar_index) const;

  void draw_and_update();

  void draw_labels() const;

  void draw_frame_and_values();

  void draw_temperature_curve() const;

  void draw_precipitation_histogram() const;

  void draw_sunrise_sunset_lines() const;

  const ibitmap *rotate_direction_icon(int degree);
};

} // namespace taranis
