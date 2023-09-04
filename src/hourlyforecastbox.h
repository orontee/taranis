#pragma once

#pragma once

#include <cmath>
#include <gsl/gsl_interp.h>
#include <inkview.h>
#include <map>
#include <memory>
#include <sstream>

#include "fonts.h"
#include "icons.h"
#include "model.h"
#include "util.h"
#include "widget.h"
#include "units.h"

#define T(x) GetLangText(x)

namespace taranis {

class HourlyForecastBox : public Widget {
public:
  HourlyForecastBox(int pos_x, int pos_y, int width, int height,
                    std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                    std::shared_ptr<Fonts> fonts)
      : Widget{pos_x, pos_y, width, height}, model{model}, icons{icons},
        fonts{fonts} {
    this->visible_bars =
        static_cast<size_t>(std::ceil(width / this->bar_width));

    this->adjusted_bar_width =
        static_cast<int>(std::ceil(width / this->visible_bars));
  }

  void show() override {
    this->fill_bounding_box();

    this->draw_frame_and_values();
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

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  const int vertical_padding{25};
  const int bar_width{125};
  const int icon_size{100};

  size_t visible_bars;
  int adjusted_bar_width;

  size_t forecast_offset{0};

  void draw_and_update() {
    this->show();

    PartialUpdate(this->bounding_box.x, this->bounding_box.y,
                  this->bounding_box.w, this->bounding_box.h);
  }

  void draw_frame_and_values() {
    const auto bar_height = this->bounding_box.h - 2 * this->vertical_padding;

    const auto start_x = this->bounding_box.x;
    const auto start_y = this->bounding_box.y + this->vertical_padding;

    DrawLine(start_x, start_y, this->bounding_box.w, start_y, LGRAY);
    DrawLine(start_x, start_y + bar_height, this->bounding_box.w,
             start_y + bar_height, LGRAY);

    auto normal_font = this->fonts->get_normal_font();
    auto small_bold_font = this->fonts->get_small_bold_font();
    auto tiny_font = this->fonts->get_tiny_font();

    const auto time_y = start_y + this->vertical_padding / 2;
    const auto icon_y = time_y + tiny_font->height;
    const auto temperature_y = start_y + bar_height -
                               this->vertical_padding / 2 -
                               normal_font->height - 2 * tiny_font->height;
    const auto wind_speed_y = start_y + bar_height -
                              this->vertical_padding / 2 -
                              2 * tiny_font->height;
    const auto humidity_y =
        start_y + bar_height - this->vertical_padding / 2 - tiny_font->height;

    const auto separator_start_y = start_y;
    const auto separator_stop_y = start_y + bar_height;

    const Units units{this->model};

    std::map<int, int> sample;
    for (size_t bar_index = 0; bar_index < this->visible_bars; ++bar_index) {
      const auto bar_center_x =
          (bar_index + 1.0 / 2) * this->adjusted_bar_width;

      if (this->forecast_offset + bar_index <
          this->model->hourly_forecast.size()) {
        const auto forecast =
            this->model->hourly_forecast[this->forecast_offset + bar_index];
        SetFont(tiny_font.get(), BLACK);

        std::string time_text{"?????"};
        const char *const time_format = "%H:%M";
        std::strftime(const_cast<char *>(time_text.c_str()), 6, time_format,
                      std::localtime(&forecast.date));
        DrawString(bar_center_x - StringWidth(time_text.c_str()) / 2.0, time_y,
                   time_text.c_str());

        DrawBitmap(bar_center_x - this->icon_size / 2.0, icon_y,
                   this->icons->get(forecast.weather_icon_name));

        SetFont(tiny_font.get(), DGRAY);

        const auto wind_speed_text = units.format_speed(forecast.wind_speed);
        DrawString(bar_center_x -
                       StringWidth(wind_speed_text.c_str()) / 2.0,
                   wind_speed_y, wind_speed_text.c_str());

        std::stringstream humidity_text;
        humidity_text << static_cast<int>(forecast.humidity) << "%";
        DrawString(bar_center_x -
                       StringWidth(humidity_text.str().c_str()) / 2.0,
                   humidity_y, humidity_text.str().c_str());

        SetFont(small_bold_font.get(), BLACK);

        const auto temperature_text = units.format_temperature(forecast.temperature);
        DrawString(bar_center_x - StringWidth(temperature_text.c_str()) / 2.0,
                   temperature_y, temperature_text.c_str());
      }

      if (bar_index < this->visible_bars - 1) {
        const auto separator_x = (bar_index + 1) * this->adjusted_bar_width;
        DrawLine(separator_x, separator_start_y, separator_x, separator_stop_y,
                 LGRAY);
      }
    }

    const auto curve_y_offset = temperature_y - this->vertical_padding;
    const auto curve_height =
        temperature_y - icon_y - this->icon_size - 2 * this->vertical_padding;
    this->draw_curve(curve_y_offset, curve_height);
  }

  void draw_curve(const int y_offset, const int height) {
    const auto ya =
        normalize_temperatures(this->model->hourly_forecast, height);
    if (ya.size() < gsl_interp_type_min_size(gsl_interp_cspline)) {
      return;
    }

    const auto step = this->adjusted_bar_width;
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
      if (x < xa.front() || x > xa.back()) {
        continue;
      }
      double y;
      const auto error = gsl_interp_eval_e(state.get(), xa.data(), ya.data(), x,
                                           accelerator.get(), &y);
      if (error) {
        // TODO log
        break;
      }

      const auto y_screen = y_offset - y;
      DrawPixel(x_screen, y_screen, BLACK);
      DrawPixel(x_screen, y_screen + 1, BLACK);
      DrawPixel(x_screen, y_screen + 2, DGRAY);
      DrawPixel(x_screen, y_screen + 3, DGRAY);
    }
  }

  void increase_forecast_offset() {
    const size_t max_forecast_offset{24 - this->visible_bars};
    const auto updated_forecast_offset = std::min(
        this->forecast_offset + this->visible_bars, max_forecast_offset);
    if (updated_forecast_offset != this->forecast_offset) {
      this->forecast_offset = updated_forecast_offset;
      this->draw_and_update();
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
    }
  }
};

} // namespace taranis
