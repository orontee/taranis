#include "hourlyforecastbox.h"

#include <boost/log/trivial.hpp>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <gsl/gsl_interp.h>
#include <inkview.h>
#include <map>
#include <memory>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <sstream>

#include "units.h"
#include "util.h"

namespace taranis {

HourlyForecastBox::HourlyForecastBox(int pos_x, int pos_y, int width,
                                     int height, std::shared_ptr<Model> model,
                                     std::shared_ptr<Icons> icons,
                                     std::shared_ptr<Fonts> fonts)
    : Widget{pos_x, pos_y, width, height}, model{model}, icons{icons},
      fonts{fonts} {
  auto normal_font = this->fonts->get_normal_font();
  auto small_bold_font = this->fonts->get_small_bold_font();
  auto tiny_font = this->fonts->get_tiny_font();

  this->bar_width =
      static_cast<int>(std::ceil(width / HourlyForecastBox::visible_bars));
  this->bars_height = this->bounding_box.h;

  this->frame_start_x = this->bounding_box.x;
  this->frame_start_y = this->bounding_box.y;

  this->date_labels_start_y = this->frame_start_y + this->vertical_padding / 2;
  this->time_y = this->date_labels_start_y + tiny_font->height;
  this->weather_icon_y = this->time_y + tiny_font->height;
  this->temperature_y = this->frame_start_y + this->bars_height -
                        this->vertical_padding / 2 - normal_font->height -
                        2 * tiny_font->height -
                        HourlyForecastBox::wind_direction_icon_size;
  this->wind_direction_icon_y = this->temperature_y + normal_font->height;
  this->wind_speed_y =
      this->wind_direction_icon_y + HourlyForecastBox::wind_direction_icon_size;
  this->wind_gust_y = this->wind_speed_y + tiny_font->height;

  this->curve_y_offset = this->temperature_y - this->vertical_padding;
  this->curve_height = this->temperature_y - this->weather_icon_y -
                       this->icon_size - 2 * this->vertical_padding;
}

void HourlyForecastBox::do_paint() {
  this->draw_labels();
  this->draw_frame_and_values();

  this->draw_sunrise_sunset_lines();
  this->draw_precipitation_histogram();
  // ⚠️ Draw sunrise and sunset lines before precipitation histogram
  // otherwise vertical lines related to sun may overlap the histogram
  // bars… See also draw_precipitation_histogram() implementation for
  // more ugly stuff related to that!

  this->draw_temperature_curve();
}

bool HourlyForecastBox::handle_key_press(int key) {
  return (key == IV_KEY_PREV or key == IV_KEY_NEXT);
}

bool HourlyForecastBox::handle_key_release(int key) {
  if (key == IV_KEY_PREV) {
    this->decrease_forecast_offset();
    return true;
  } else if (key == IV_KEY_NEXT) {
    this->increase_forecast_offset();
    return true;
  }
  return false;
}

size_t HourlyForecastBox::page_count() const {
  return (this->model->hourly_forecast.size() /
          HourlyForecastBox::visible_bars) +
         (this->model->hourly_forecast.size() %
                      HourlyForecastBox::visible_bars ==
                  0
              ? 0
              : 1);
}

size_t HourlyForecastBox::current_page() const {
  return (this->get_forecast_offset() / HourlyForecastBox::visible_bars);
}

size_t HourlyForecastBox::get_forecast_offset() const {
  return this->forecast_offset;
}

size_t HourlyForecastBox::get_min_forecast_offset() const { return 0; }

size_t HourlyForecastBox::get_max_forecast_offset() const {
  return this->model->hourly_forecast.size() - HourlyForecastBox::visible_bars;
}

void HourlyForecastBox::set_min_forecast_offset() {
  if (this->forecast_offset == 0) {
    return;
  }
  this->forecast_offset = 0;
  BOOST_LOG_TRIVIAL(debug) << "Forecast offset set to its min value";
  this->paint_and_update_screen();
  this->on_page_changed();
}

void HourlyForecastBox::set_max_forecast_offset() {
  const size_t max_forecast_offset{this->model->hourly_forecast.size() -
                                   HourlyForecastBox::visible_bars};
  if (this->forecast_offset == max_forecast_offset) {
    return;
  }
  this->forecast_offset = max_forecast_offset;
  BOOST_LOG_TRIVIAL(debug) << "Forecast offset set to its max value";
  this->paint_and_update_screen();
  this->on_page_changed();
}

void HourlyForecastBox::increase_forecast_offset() {
  const size_t max_forecast_offset{this->model->hourly_forecast.size() -
                                   HourlyForecastBox::visible_bars};
  const auto updated_forecast_offset =
      std::min(this->forecast_offset + HourlyForecastBox::visible_bars,
               max_forecast_offset);
  if (updated_forecast_offset != this->forecast_offset) {
    this->forecast_offset = updated_forecast_offset;
    BOOST_LOG_TRIVIAL(debug)
        << "Forecast offset increased to " << this->forecast_offset;
    this->paint_and_update_screen();
    this->on_page_changed();
  }
}

void HourlyForecastBox::decrease_forecast_offset() {
  const size_t min_forecast_offset{0};
  const auto updated_forecast_offset =
      (this->forecast_offset > HourlyForecastBox::visible_bars)
          ? this->forecast_offset - HourlyForecastBox::visible_bars
          : min_forecast_offset;

  // don't use std::max since we're working with unsigned integers!

  if (updated_forecast_offset != this->forecast_offset) {
    this->forecast_offset = updated_forecast_offset;
    BOOST_LOG_TRIVIAL(debug)
        << "Forecast offset decreased to " << this->forecast_offset;
    this->paint_and_update_screen();
    this->on_page_changed();
  }
}

std::tuple<std::string, int>
HourlyForecastBox::get_date_label_properties(size_t bar_index) const {
  const int forecast_index = this->forecast_offset + bar_index;
  if (forecast_index < this->model->hourly_forecast.size()) {
    const auto forecast = this->model->hourly_forecast[forecast_index];
    if (bar_index == 0) {
      return {format_date(forecast.date, true),
              std::min(static_cast<int>(HourlyForecastBox::visible_bars),
                       remaining_hours(forecast.date)) *
                  this->bar_width};
    } else if (bar_index == HourlyForecastBox::visible_bars - 1) {
      return {format_date(forecast.date, true),
              std::min(static_cast<int>(HourlyForecastBox::visible_bars),
                       (passed_hours(forecast.date) + 1)) *
                  this->bar_width};
    }
  }
  return {"", 0};
}

void HourlyForecastBox::draw_labels() const {
  auto tiny_font = this->fonts->get_tiny_font();
  SetFont(tiny_font.get(), BLACK);

  const auto [left_label_text, left_label_width] =
      this->get_date_label_properties(0);
  if (not left_label_text.empty() and left_label_width > 0) {
    if (StringWidth(left_label_text.c_str()) < left_label_width) {
      DrawTextRect(this->bounding_box.x, this->date_labels_start_y,
                   left_label_width, tiny_font->height, left_label_text.c_str(),
                   ALIGN_CENTER);
    };
  }

  const auto [right_label_text, right_label_width] =
      this->get_date_label_properties(HourlyForecastBox::visible_bars - 1);
  if (not right_label_text.empty() and right_label_width > 0 and
      right_label_text != left_label_text) {
    if (StringWidth(right_label_text.c_str()) < right_label_width) {
      DrawTextRect(this->bounding_box.x + this->bounding_box.w -
                       right_label_width,
                   this->date_labels_start_y, right_label_width,
                   tiny_font->height, right_label_text.c_str(), ALIGN_CENTER);
    };
  }
}

void HourlyForecastBox::draw_frame_and_values() {
  DrawLine(this->frame_start_x, this->frame_start_y, this->bounding_box.w,
           this->frame_start_y, LGRAY);
  DrawLine(this->frame_start_x, this->frame_start_y + this->bars_height,
           this->bounding_box.w, this->frame_start_y + this->bars_height,
           LGRAY);

  const auto normal_font = this->fonts->get_normal_font();
  const auto small_bold_font = this->fonts->get_small_bold_font();
  const auto tiny_font = this->fonts->get_tiny_font();
  const auto tiny_italic_font = this->fonts->get_tiny_italic_font();

  const auto separator_stop_y = this->frame_start_y + this->bars_height;

  const Units units{this->model};

  for (size_t bar_index = 0; bar_index < HourlyForecastBox::visible_bars;
       ++bar_index) {
    const auto bar_center_x = (bar_index + 1.0 / 2) * this->bar_width;
    auto separator_start_y = this->time_y;

    const auto forecast_index = this->forecast_offset + bar_index;
    if (forecast_index < this->model->hourly_forecast.size()) {
      const auto forecast = this->model->hourly_forecast[forecast_index];

      if (remaining_hours(forecast.date) == 1) {
        separator_start_y = this->frame_start_y;
      }

      SetFont(tiny_font.get(), BLACK);

      const auto time_text = format_time(forecast.date, true);
      DrawString(bar_center_x - StringWidth(time_text.c_str()) / 2.0,
                 this->time_y, time_text.c_str());

      DrawBitmap(bar_center_x - this->icon_size / 2.0, this->weather_icon_y,
                 this->icons->get(forecast.weather_icon_name));

      SetFont(small_bold_font.get(), BLACK);

      const auto temperature_text =
          units.format_temperature(forecast.temperature);
      DrawString(bar_center_x - StringWidth(temperature_text.c_str()) / 2.0,
                 this->temperature_y, temperature_text.c_str());

      if (static_cast<int>(forecast.wind_speed) != 0 or
          static_cast<int>(forecast.wind_gust) != 0) {
        const auto rotated_icon =
            this->rotate_direction_icon(forecast.wind_degree);
        if (rotated_icon) {
          DrawBitmap(bar_center_x - this->wind_direction_icon_size / 2.0,
                     this->wind_direction_icon_y, rotated_icon);
        }
      }
      const auto wind_speed_text = units.format_speed(forecast.wind_speed);
      if (static_cast<int>(forecast.wind_speed) != 0) {
        SetFont(tiny_font.get(), DGRAY);
        DrawString(bar_center_x - StringWidth(wind_speed_text.c_str()) / 2.0,
                   this->wind_speed_y, wind_speed_text.c_str());
      }
      if (static_cast<int>(forecast.wind_gust) != 0) {
        const auto wind_gust_text = units.format_speed(forecast.wind_gust);
        if (forecast.wind_gust > forecast.wind_speed and
            wind_gust_text != wind_speed_text) {
          SetFont(tiny_italic_font.get(), DGRAY);
          DrawString(bar_center_x - StringWidth(wind_gust_text.c_str()) / 2.0,
                     this->wind_gust_y, wind_gust_text.c_str());
        }
      }
    }

    if (bar_index < HourlyForecastBox::visible_bars - 1) {
      const auto separator_x = (bar_index + 1) * this->bar_width;
      DrawLine(separator_x, separator_start_y, separator_x, separator_stop_y,
               LGRAY);
    }
  }
}

void HourlyForecastBox::draw_temperature_curve() const {
  const auto ya =
      normalize_temperatures(this->model->hourly_forecast, this->curve_height);
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

void HourlyForecastBox::draw_precipitation_histogram() const {
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

  for (size_t bar_index = 0; bar_index < HourlyForecastBox::visible_bars;
       ++bar_index) {
    const auto forecast_index = this->forecast_offset + bar_index;
    if (forecast_index < this->model->hourly_forecast.size()) {
      const auto forecast = this->model->hourly_forecast[forecast_index];
      const auto precipitation = max_number(forecast.rain, forecast.snow);
      if ((std::isnan(forecast.rain) and std::isnan(forecast.snow) or
          precipitation == 0)) {
        continue;
      }
      const auto bar_center_x = (bar_index + 1.0 / 2) * this->bar_width;
      const auto x_screen = this->bounding_box.x + bar_index * this->bar_width;
      const auto bar_height = normalized_precipitations.at(forecast_index);
      const auto y_screen = this->curve_y_offset - bar_height;
      FillArea(x_screen, y_screen, this->bar_width, bar_height, LGRAY);
      DrawRect(x_screen, y_screen, this->bar_width, bar_height, 0x777777);

      const auto precipitation_text = units.format_precipitation(
          max_number(forecast.rain, forecast.snow), false);
      // rain and snow are in mm/h but save space with shortest unit

      FillArea(x_screen + 1,
               y_screen - tiny_font.get()->height - this->vertical_padding,
               this->bar_width - 2, tiny_font.get()->height - 1, WHITE);
      // ⚠️ Clear the bottom of a possible vertical line
      // corresponding to sunrise or sunset

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
        DrawString(bar_center_x -
                       StringWidth(probability_of_precipitation_text.c_str()) /
                           2.0,
                   this->curve_y_offset - tiny_font.get()->height - 2,
                   probability_of_precipitation_text.c_str());
      }
    }
  }
}

void HourlyForecastBox::draw_sunrise_sunset_lines() const {
  BOOST_LOG_TRIVIAL(debug) << "Drawing sunrise and sunset lines";

  const auto icon_start_y = this->weather_icon_y + this->icon_size;
  const auto separator_start_y = icon_start_y + this->icon_size;
  const auto separator_stop_y = this->curve_y_offset;

  for (size_t bar_index = 0; bar_index < HourlyForecastBox::visible_bars;
       ++bar_index) {
    const auto forecast_index = this->forecast_offset + bar_index;
    if (forecast_index < this->model->hourly_forecast.size()) {
      const auto forecast = this->model->hourly_forecast[forecast_index];

      for (const auto daily_forecast : this->model->daily_forecast) {
        if (not are_same_day(forecast.date, daily_forecast.date)) {
          continue;
        }

        const auto minutes_before_sunrise =
            get_duration_minutes(forecast.date, daily_forecast.sunrise);

        const auto minutes_before_sunset =
            get_duration_minutes(forecast.date, daily_forecast.sunset);

        if ((0 <= minutes_before_sunrise and minutes_before_sunrise < 60) or
            (minutes_before_sunrise == 60 and
             bar_index + 1 == HourlyForecastBox::visible_bars)) {
          const int separator_x = bar_index * this->bar_width +
                                  minutes_before_sunrise * this->bar_width / 60;
          BOOST_LOG_TRIVIAL(debug) << "Drawing sunrise line";
          DrawDashLine(separator_x, separator_start_y, separator_x,
                       separator_stop_y, LGRAY, 10, 10);

          DrawBitmap(separator_x - this->icon_size / 2.0, icon_start_y,
                     this->icons->get("sunrise"));

        } else if ((0 <= minutes_before_sunset and
                    minutes_before_sunset < 60) or
                   (minutes_before_sunset == 60 and
                    bar_index + 1 == HourlyForecastBox::visible_bars)) {
          const int separator_x = bar_index * this->bar_width +
                                  minutes_before_sunset * this->bar_width / 60;
          BOOST_LOG_TRIVIAL(debug) << "Drawing sunset line";
          DrawDashLine(separator_x, separator_start_y, separator_x,
                       separator_stop_y, LGRAY, 10, 10);

          DrawBitmap(separator_x - this->icon_size / 2.0, icon_start_y,
                     this->icons->get("sunset"));
        }
      }
    } else {
      BOOST_LOG_TRIVIAL(debug) << "Out-of-range bar index";
    }
  }
}

const ibitmap *HourlyForecastBox::rotate_direction_icon(int degree) {
  const auto arrow_angle = (180 - degree);
  // The parameter degree is an angle measure in degrees, interpreted
  // as the direction where the wind is blowing FROM (0 means North,
  // 90 East), but the icon arrow must show where the wind is blowing
  // TO. Whats more OpenCV rotation is counter-clockwise for positive
  // angle values.

  auto &rotated_icon_data = this->icons->rotate_icon(
      "direction", HourlyForecastBox::wind_direction_icon_size, arrow_angle);
  if (not rotated_icon_data.empty()) {
    return reinterpret_cast<const ibitmap *>(rotated_icon_data.data());
  }
  return nullptr;
}
} // namespace taranis
