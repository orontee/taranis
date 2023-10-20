#pragma once

#pragma once

#include <cmath>
#include <gsl/gsl_interp.h>
#include <inkview.h>
#include <map>
#include <memory>
#include <sstream>

#include "events.h"
#include "fonts.h"
#include "icons.h"
#include "model.h"
#include "units.h"
#include "util.h"
#include "widget.h"

namespace taranis {

class HourlyForecastBox : public Widget {
public:
  HourlyForecastBox(int pos_x, int pos_y, int width, int height,
                    std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                    std::shared_ptr<Fonts> fonts)
      : Widget{pos_x, pos_y, width, height}, model{model}, icons{icons},
        fonts{fonts} {
    this->visible_bars = 8;

    this->bar_width = static_cast<int>(std::ceil(width / this->visible_bars));

    this->bars_height = this->bounding_box.h;

    this->frame_start_x = this->bounding_box.x;
    this->frame_start_y = this->bounding_box.y;

    auto normal_font = this->fonts->get_normal_font();
    auto small_bold_font = this->fonts->get_small_bold_font();
    auto tiny_font = this->fonts->get_tiny_font();

    this->time_y = this->frame_start_y + this->vertical_padding / 2;
    this->icon_y = this->time_y + tiny_font->height;
    this->temperature_y = this->frame_start_y + this->bars_height -
                          this->vertical_padding / 2 - normal_font->height -
                          2 * tiny_font->height;
    this->wind_speed_y = this->temperature_y + normal_font->height;
    this->humidity_y = this->wind_speed_y + tiny_font->height;

    this->curve_y_offset = this->temperature_y - this->vertical_padding;
    this->curve_height = this->temperature_y - this->icon_y - this->icon_size -
                         2 * this->vertical_padding;
  }

  void show() override {
    this->fill_bounding_box();

    this->draw_frame_and_values();
    this->draw_precipitation_histogram();
    this->draw_temperature_curve();
  }

  int handle_key_pressed(int key) override {
    if (key == IV_KEY_PREV) {
      this->decrease_forecast_offset();
      return 1;
    }

    if (key == IV_KEY_NEXT) {
      this->increase_forecast_offset();
      return 1;
    }
    return 0;
  }

  void increase_forecast_offset() {
    const size_t max_forecast_offset{24 - this->visible_bars};
    const auto updated_forecast_offset = std::min(
        this->forecast_offset + this->visible_bars, max_forecast_offset);
    if (updated_forecast_offset != this->forecast_offset) {
      this->forecast_offset = updated_forecast_offset;
      this->draw_and_update();
    } else {
      this->forecast_offset = 0;
      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM,
                CustomEvent::change_daily_forecast_display, 0);
    }
  }

  void decrease_forecast_offset() {
    const size_t min_forecast_offset{0};
    const auto updated_forecast_offset =
        (this->forecast_offset > this->visible_bars)
            ? this->forecast_offset - this->visible_bars
            : min_forecast_offset;

    // don't use std::max since we're working with unsigned integers!

    if (updated_forecast_offset != this->forecast_offset) {
      this->forecast_offset = updated_forecast_offset;
      this->draw_and_update();
    } else {
      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM,
                CustomEvent::change_daily_forecast_display, 0);
    }
  }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  const int vertical_padding{25};
  const int icon_size{100};

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

  void draw_and_update() {
    this->show();

    PartialUpdate(this->bounding_box.x, this->bounding_box.y,
                  this->bounding_box.w, this->bounding_box.h);
  }

  void draw_frame_and_values() const {
    DrawLine(this->frame_start_x, this->frame_start_y, this->bounding_box.w,
             this->frame_start_y, LGRAY);
    DrawLine(this->frame_start_x, this->frame_start_y + this->bars_height,
             this->bounding_box.w, this->frame_start_y + this->bars_height,
             LGRAY);

    auto normal_font = this->fonts->get_normal_font();
    auto small_bold_font = this->fonts->get_small_bold_font();
    auto tiny_font = this->fonts->get_tiny_font();

    const auto separator_start_y = this->frame_start_y;
    const auto separator_stop_y = this->frame_start_y + this->bars_height;

    const Units units{this->model};

    for (size_t bar_index = 0; bar_index < this->visible_bars; ++bar_index) {
      const auto bar_center_x = (bar_index + 1.0 / 2) * this->bar_width;

      const auto forecast_index = this->forecast_offset + bar_index;
      if (forecast_index < this->model->hourly_forecast.size()) {
        const auto forecast = this->model->hourly_forecast[forecast_index];

        SetFont(tiny_font.get(), BLACK);

        const auto time_text = format_time(forecast.date, true);
        DrawString(bar_center_x - StringWidth(time_text.c_str()) / 2.0,
                   this->time_y, time_text.c_str());

        DrawBitmap(bar_center_x - this->icon_size / 2.0, this->icon_y,
                   this->icons->get(forecast.weather_icon_name));

        SetFont(small_bold_font.get(), BLACK);

        const auto temperature_text =
            units.format_temperature(forecast.temperature);
        DrawString(bar_center_x - StringWidth(temperature_text.c_str()) / 2.0,
                   this->temperature_y, temperature_text.c_str());

        SetFont(tiny_font.get(), DGRAY);

        const auto wind_speed_text = units.format_speed(forecast.wind_speed);
        DrawString(bar_center_x - StringWidth(wind_speed_text.c_str()) / 2.0,
                   this->wind_speed_y, wind_speed_text.c_str());

        const auto humidity_text =
            std::to_string(static_cast<int>(forecast.humidity)) + "%";
        DrawString(bar_center_x - StringWidth(humidity_text.c_str()) / 2.0,
                   this->humidity_y, humidity_text.c_str());
      }

      if (bar_index < this->visible_bars - 1) {
        const auto separator_x = (bar_index + 1) * this->bar_width;
        DrawLine(separator_x, separator_start_y, separator_x, separator_stop_y,
                 LGRAY);
      }
    }
  }

  void draw_temperature_curve() const {
    const auto ya = normalize_temperatures(this->model->hourly_forecast,
                                           this->curve_height);
    if (ya.size() < gsl_interp_type_min_size(gsl_interp_cspline)) {
      return;
    }

    const auto step = this->bar_width;
    std::vector<double> xa;
    xa.reserve(ya.size());
    for (size_t index = 0; index < ya.size(); ++index) {
      xa.push_back(index * step + step / 2.0);
    }

    std::unique_ptr<gsl_interp_accel, void (*)(gsl_interp_accel *)> accelerator{
        gsl_interp_accel_alloc(), &gsl_interp_accel_free};
    std::unique_ptr<gsl_interp, void (*)(gsl_interp *)> state{
        gsl_interp_alloc(gsl_interp_cspline, xa.size()), &gsl_interp_free};

    const auto error =
        gsl_interp_init(state.get(), xa.data(), ya.data(), xa.size());
    if (error) {
      // TODO log
      return;
    }

    const auto width = this->bounding_box.w;
    for (int x_screen = this->bounding_box.x; x_screen < width; ++x_screen) {
      const double x =
          this->forecast_offset * step + (x_screen - this->bounding_box.x);
      if (x < xa.front() or x > xa.back()) {
        continue;
      }
      double y;
      const auto error = gsl_interp_eval_e(state.get(), xa.data(), ya.data(), x,
                                           accelerator.get(), &y);
      if (error) {
        // TODO log
        break;
      }

      const auto y_screen = this->curve_y_offset - y;
      DrawPixel(x_screen, y_screen, BLACK);
      DrawPixel(x_screen, y_screen + 1, BLACK);
      DrawPixel(x_screen, y_screen + 2, DGRAY);
      DrawPixel(x_screen, y_screen + 3, DGRAY);
    }
  }

  void draw_precipitation_histogram() const {
    auto tiny_font = this->fonts->get_tiny_font();
    const auto min_bars_height = tiny_font.get()->height + 10;
    // 2 pixels of padding in bar
    const auto max_bars_height = this->curve_height - tiny_font.get()->height;
    // 1 pixel of padding between bar top and
    // probability_of_precipitation text

    if (max_bars_height <= min_bars_height) {
      return;
    }

    const auto normalized_precipitations = normalize_precipitations(
        this->model->hourly_forecast, min_bars_height, max_bars_height);

    const Units units{this->model};

    SetFont(tiny_font.get(), DGRAY);

    for (size_t bar_index = 0; bar_index < this->visible_bars; ++bar_index) {
      const auto forecast_index = this->forecast_offset + bar_index;
      if (forecast_index < this->model->hourly_forecast.size()) {
        const auto forecast = this->model->hourly_forecast[forecast_index];
        if (std::isnan(forecast.rain)) {
          continue;
        }
        const auto bar_center_x = (bar_index + 1.0 / 2) * this->bar_width;
        const auto x_screen =
            this->bounding_box.x + bar_index * this->bar_width;
        const auto bar_height = normalized_precipitations.at(forecast_index);
        const auto y_screen = this->curve_y_offset - bar_height;
        FillArea(x_screen, y_screen, this->bar_width, bar_height, LGRAY);
        DrawRect(x_screen, y_screen, this->bar_width, bar_height, 0x777777);

        const auto precipitation_text = units.format_precipitation(
            max_number(forecast.rain, forecast.snow), false);
        // rain and snow are in mm/h but save space with shortest unit

        SetFont(tiny_font.get(), DGRAY);
        DrawString(bar_center_x - StringWidth(precipitation_text.c_str()) / 2.0,
                   y_screen - tiny_font.get()->height - 2,
                   precipitation_text.c_str());

        const auto probability_of_precipitation =
            forecast.probability_of_precipitation;
        if (not std::isnan(probability_of_precipitation)) {

          const auto probability_of_precipitation_text =
              std::to_string(
                  static_cast<int>(probability_of_precipitation * 100)) +
              "%";

          SetFont(tiny_font.get(), BLACK);
          DrawString(
              bar_center_x -
                  StringWidth(probability_of_precipitation_text.c_str()) / 2.0,
              this->curve_y_offset - tiny_font.get()->height - 2,
              probability_of_precipitation_text.c_str());
        }
      }
    }
  }
};

} // namespace taranis
