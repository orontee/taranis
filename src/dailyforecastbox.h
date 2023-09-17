#pragma once

#pragma once

#include <algorithm>
#include <inkview.h>
#include <memory>

#include "events.h"
#include "fonts.h"
#include "icons.h"
#include "model.h"
#include "units.h"
#include "util.h"
#include "widget.h"

#define T(x) GetLangText(x)

using namespace std::string_literals;

namespace taranis {

class DailyForecastBox : public Widget {
public:
  DailyForecastBox(int pos_x, int pos_y, int width, int height,
                   std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                   std::shared_ptr<Fonts> fonts)
      : Widget{pos_x, pos_y, width, height}, model{model}, icons{icons},
        fonts{fonts} {
    this->bar_height = this->bounding_box.h / DailyForecastBox::max_forecasts;
  }

  void show() override {
    this->fill_bounding_box();

    this->draw_frame_and_values();
  }

  int handle_key_pressed(int key) override {
    if (key == IV_KEY_PREV or key == IV_KEY_NEXT) {
      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM,
                CustomEvent::change_daily_forecast_display, 0);
      return 1;
    }
    return 0;
  }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  static const int max_forecasts{8};
  static const int horizontal_padding{25};

  int bar_height;

  int compute_day_pixel_width() {
    static std::array<const char *, 7> weekdays = {
        "@Sun", "@Mon", "@Tue", "@Wed", "@Thu", "@Fri", "@Sat"};
    static std::array<int, 7> weekdays_width;

    auto small_font = this->fonts->get_small_font();
    SetFont(small_font.get(), BLACK);

    std::transform(
        std::begin(weekdays), std::end(weekdays), std::begin(weekdays_width),
        [](const char *weekday) { return StringWidth(GetLangText(weekday)); });
    return *std::max_element(std::begin(weekdays_width),
                             std::end(weekdays_width));
  }

  void draw_frame_and_values() {
    const auto bar_start_x = this->bounding_box.x;
    auto bar_start_y = this->bounding_box.y;
    const auto separator_start_x = bar_start_x;
    auto separator_start_y = bar_start_y;
    const auto separator_stop_x = this->bounding_box.w;
    DrawLine(separator_start_x, separator_start_y, separator_stop_x,
             separator_start_y, LGRAY);

    auto normal_font = this->fonts->get_normal_font();
    auto small_font = this->fonts->get_small_font();
    auto small_bold_font = this->fonts->get_small_bold_font();
    auto tiny_font = this->fonts->get_tiny_font();

    const auto day_start_x =
        bar_start_x + 2 * DailyForecastBox::horizontal_padding;
    const auto icon_start_x = day_start_x + this->compute_day_pixel_width() +
                              DailyForecastBox::horizontal_padding / 2;
    // Can't be computed once since depends on current language

    const Units units{this->model};

    for (size_t bar_index = 0; bar_index < DailyForecastBox::max_forecasts;
         ++bar_index) {
      bar_start_y = this->bounding_box.y + bar_index * this->bar_height;

      const auto forecast_index = bar_index;
      if (forecast_index < this->model->daily_forecast.size()) {
        const auto forecast = this->model->daily_forecast[forecast_index];

        const auto bar_center_y = bar_start_y + this->bar_height / 2;
        const auto bar_text_y =
            bar_start_y + (this->bar_height - small_font->height) / 2;
        const auto upper_text_y =
            bar_start_y + (this->bar_height - 2 * tiny_font->height) / 3;
        const auto lower_text_y =
            upper_text_y + tiny_font->height +
            (this->bar_height - 2 * tiny_font->height) / 3;

        const auto day_text = format_day(forecast.date);
        SetFont(small_font.get(), BLACK);
        DrawString(day_start_x, bar_text_y, day_text.c_str());

        const auto icon = this->icons->get(forecast.weather_icon_name);
        const auto icon_start_y = bar_center_y - icon->height / 2;
        DrawBitmap(icon_start_x, icon_start_y, icon);

        const auto sunrise_start_x =
            icon_start_x + icon->width + DailyForecastBox::horizontal_padding / 2;
        const auto sunrise_text = format_time(forecast.sunrise);
        SetFont(tiny_font.get(), BLACK);
        DrawString(sunrise_start_x, lower_text_y, sunrise_text.c_str());

        const auto sunset_start_x = sunrise_start_x;
        const auto sunset_text = format_time(forecast.sunset);
        DrawString(sunset_start_x, upper_text_y, sunset_text.c_str());

        const auto morning_temperature_text =
            units.format_temperature(forecast.temperature_morning);
        const auto morning_temperature_start_x =
            sunrise_start_x + StringWidth(sunrise_text.c_str()) +
            DailyForecastBox::horizontal_padding;

        SetFont(small_bold_font.get(), BLACK);

        DrawString(morning_temperature_start_x, bar_text_y,
                   morning_temperature_text.c_str());

        const auto day_temperature_text =
            units.format_temperature(forecast.temperature_day);
        const auto day_temperature_start_x =
            morning_temperature_start_x +
            StringWidth(morning_temperature_text.c_str()) +
            DailyForecastBox::horizontal_padding;
        DrawString(day_temperature_start_x, bar_text_y,
                   day_temperature_text.c_str());

        const auto evening_temperature_text =
            units.format_temperature(forecast.temperature_evening);
        const auto evening_temperature_start_x =
            day_temperature_start_x +
            StringWidth(day_temperature_text.c_str()) +
            DailyForecastBox::horizontal_padding;
        DrawString(evening_temperature_start_x, bar_text_y,
                   evening_temperature_text.c_str());

        const auto min_temperature_start_x =
            evening_temperature_start_x +
            StringWidth(evening_temperature_text.c_str()) +
            DailyForecastBox::horizontal_padding;
        const auto min_temperature_text =
            units.format_temperature(forecast.temperature_min);
        SetFont(tiny_font.get(), BLACK);
        DrawString(min_temperature_start_x, lower_text_y,
                   min_temperature_text.c_str());

        const auto max_temperature_start_x = min_temperature_start_x;
        const auto max_temperature_text =
            units.format_temperature(forecast.temperature_max);
        DrawString(max_temperature_start_x, upper_text_y,
                   max_temperature_text.c_str());

        const auto wind_speed_start_x =
            max_temperature_start_x +
            std::max(StringWidth(min_temperature_text.c_str()),
                     StringWidth(max_temperature_text.c_str())) +
            DailyForecastBox::horizontal_padding;
        const auto wind_speed_text = units.format_speed(forecast.wind_speed);
        DrawString(wind_speed_start_x, upper_text_y, wind_speed_text.c_str());

        const auto humidity_text =
            std::to_string(static_cast<int>(forecast.humidity)) + "%";
        DrawString(wind_speed_start_x, lower_text_y, humidity_text.c_str());

        const auto precipitation = max_number(forecast.rain, forecast.snow);
        if (not std::isnan(precipitation)) {
          const auto precipitation_text =
              units.format_precipitation(precipitation, false);

          const auto precipitation_start_y =
              ScreenWidth() - 2 * DailyForecastBox::horizontal_padding -
              StringWidth(precipitation_text.c_str());
          DrawString(precipitation_start_y, upper_text_y,
                     precipitation_text.c_str());

          const auto probability_of_precipitation =
              forecast.probability_of_precipitation;
          if (not std::isnan(probability_of_precipitation)) {
            const auto probability_of_precipitation_text =
                std::to_string(
                    static_cast<int>(probability_of_precipitation * 100)) +
                "%";
            const auto probability_of_precipitation_start_y =
                ScreenWidth() - 2 * DailyForecastBox::horizontal_padding -
                StringWidth(probability_of_precipitation_text.c_str());
            DrawString(probability_of_precipitation_start_y, lower_text_y,
                       probability_of_precipitation_text.c_str());
          }
        }
      }
      separator_start_y = bar_start_y + this->bar_height;
      DrawLine(separator_start_x, separator_start_y, separator_stop_x,
               separator_start_y, LGRAY);
    }
  }
};

} // namespace taranis
