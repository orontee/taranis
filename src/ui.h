#pragma once

#include <cmath>
#include <ctime>
#include <gsl/gsl_interp.h>
#include <inkview.h>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>

#include "icons.h"
#include "model.h"
#include "util.h"

#define T(x) GetLangText(x)

using namespace std::string_literals;

namespace taranis {

constexpr int FONT_SIZE = 50;
constexpr int BIG_FONT_SIZE = 150;
constexpr int SMALL_FONT_SIZE = 40;
constexpr int TINY_FONT_SIZE = 30;
constexpr int PADDING = 50;
constexpr int ICON_SIZE = 100;
constexpr int BAR_WIDTH = 125;

class Ui {
public:
  // ⚠️ Must be instantiated after application received the EVT_INIT
  // event otherwise opening fonts, etc. fails

  Ui(std::shared_ptr<Model> model)
      : // fonts
        font{OpenFont(DEFAULTFONT, FONT_SIZE, false), &CloseFont},
        bold_font{OpenFont(DEFAULTFONTB, FONT_SIZE, false), &CloseFont},
        big_font{OpenFont(DEFAULTFONTB, BIG_FONT_SIZE, false), &CloseFont},
        small_font{OpenFont(DEFAULTFONT, SMALL_FONT_SIZE, false), &CloseFont},
        small_bold_font{OpenFont(DEFAULTFONTB, SMALL_FONT_SIZE, false),
                        &CloseFont},
        tiny_font{OpenFont(DEFAULTFONT, TINY_FONT_SIZE, false), &CloseFont},
        model{model}, forecast_offset{0} {
    SetPanelType(0);
    SetOrientation(0);

    const auto screen_width = ScreenWidth();

    // top box is made of one line of text using bold font, one line
    // of text using big font, with padding at top and bottom
    this->top_box_height =
        (this->bold_font->height + this->big_font->height + 2 * PADDING);

    // status bar is made of two lines of text using small font,
    // with padding at top and bottom
    this->status_bar_height = 2 * this->tiny_font->height + 2 * PADDING;

    this->visible_bars =
        static_cast<size_t>(std::ceil(screen_width / BAR_WIDTH));

    this->adjusted_bar_width =
        static_cast<int>(std::ceil(screen_width / this->visible_bars));
  }

  void show() {
    ClearScreen();

    this->draw_top_box();
    this->draw_middle_box();
    this->draw_status_bar();

    FullUpdate();
  }

  void increase_forecast_offset() {
    const size_t max_forecast_offset{24 - this->visible_bars};
    const auto updated_forecast_offset = std::min(
        this->forecast_offset + this->visible_bars, max_forecast_offset);
    if (updated_forecast_offset != this->forecast_offset) {
      this->forecast_offset = updated_forecast_offset;
      this->draw_middle_box_and_update();
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
      this->draw_middle_box_and_update();
    }
  }

private:
  std::unique_ptr<ifont, void (*)(ifont *)> font;
  std::unique_ptr<ifont, void (*)(ifont *)> bold_font;
  std::unique_ptr<ifont, void (*)(ifont *)> big_font;
  std::unique_ptr<ifont, void (*)(ifont *)> small_font;
  std::unique_ptr<ifont, void (*)(ifont *)> small_bold_font;
  std::unique_ptr<ifont, void (*)(ifont *)> tiny_font;

  std::shared_ptr<Model> model;

  Icons icons;

  size_t forecast_offset;

  int top_box_height;
  int status_bar_height;

  size_t visible_bars;
  int adjusted_bar_width;

  void draw_top_box() {
    const auto screen_width = ScreenWidth();
    FillArea(0, 0, screen_width, this->top_box_height, 0x00FFFFFF);

    const auto first_row_x = PADDING;
    const auto first_row_y = PADDING;

    std::stringstream location_text;
    location_text << this->model->location.town << ", "
                  << this->model->location.country;

    SetFont(this->bold_font.get(), BLACK);
    DrawString(first_row_x, first_row_y, location_text.str().c_str());

    const auto current_condition = this->model->current_condition;
    if (not current_condition) {
      return;
    }

    const auto second_row_x = first_row_x;
    const auto second_row_y = first_row_y + GetFont()->height;

    std::stringstream temperature_text;
    temperature_text << static_cast<int>(current_condition->temperature) << "°";
    SetFont(this->big_font.get(), BLACK);
    DrawString(second_row_x, second_row_y, temperature_text.str().c_str());

    const auto third_row_x =
        (second_row_x + StringWidth(temperature_text.str().c_str()) + PADDING);

    const auto third_row_y =
        (second_row_y + (GetFont()->height - 2 * this->small_font->size));

    std::stringstream felt_temperature_text;
    felt_temperature_text << T("Felt") << " "
                          << static_cast<int>(
                                 current_condition->felt_temperature)
                          << "°";
    SetFont(this->small_font.get(), BLACK);
    DrawString(third_row_x, third_row_y, felt_temperature_text.str().c_str());
  }

  void draw_middle_box() {
    const auto screen_height = ScreenHeight();
    const auto screen_width = ScreenWidth();
    FillArea(0, this->top_box_height + 1, screen_width,
             screen_height - this->top_box_height - this->status_bar_height,
             0x00FFFFFF);

    SetFont(this->font.get(), BLACK);

    const auto bar_height = screen_height -
                            (this->top_box_height + this->status_bar_height) -
                            PADDING;

    const auto start_y = this->top_box_height + PADDING;

    DrawLine(0, start_y, screen_width, start_y, LGRAY);
    DrawLine(0, start_y + bar_height, screen_width, start_y + bar_height,
             LGRAY);

    const auto time_y = start_y + PADDING / 2;
    const auto icon_y = time_y + this->small_font->height;
    const auto temperature_y =
        (start_y + bar_height - PADDING / 2 - this->small_font->height -
         2 * this->tiny_font->height);
    const auto wind_speed_y =
        start_y + bar_height - PADDING / 2 - 2 * this->tiny_font->height;
    const auto humidity_y =
        start_y + bar_height - PADDING / 2 - this->tiny_font->height;

    const auto separator_start_y = start_y;
    const auto separator_stop_y = start_y + bar_height;

    std::map<int, int> sample;
    for (size_t bar_index = 0; bar_index < this->visible_bars; ++bar_index) {
      const auto bar_center_x =
          (bar_index + 1.0 / 2) * this->adjusted_bar_width;

      if (this->forecast_offset + bar_index <
          this->model->hourly_forecast.size()) {
        const auto forecast =
            this->model->hourly_forecast[this->forecast_offset + bar_index];
        SetFont(this->small_font.get(), BLACK);

        std::string time_text{"?????"};
        const char *const time_format = "%H:%M";
        std::strftime(const_cast<char *>(time_text.c_str()), 6, time_format,
                      std::localtime(&forecast.date));
        DrawString(bar_center_x - StringWidth(time_text.c_str()) / 2.0, time_y,
                   time_text.c_str());

        DrawBitmap(bar_center_x - ICON_SIZE / 2.0, icon_y,
                   this->icons.get(forecast.weather_icon_name));

        SetFont(this->tiny_font.get(), DGRAY);

        std::stringstream wind_speed_text;
        wind_speed_text << static_cast<int>(forecast.wind_speed) << "m/s";
        DrawString(bar_center_x -
                       StringWidth(wind_speed_text.str().c_str()) / 2.0,
                   wind_speed_y, wind_speed_text.str().c_str());

        std::stringstream humidity_text;
        humidity_text << static_cast<int>(forecast.humidity) << "%";
        DrawString(bar_center_x -
                       StringWidth(humidity_text.str().c_str()) / 2.0,
                   humidity_y, humidity_text.str().c_str());

        SetFont(this->small_bold_font.get(), BLACK);

        std::stringstream temperature_text;
        temperature_text << static_cast<int>(forecast.temperature) << "°";
        DrawString(bar_center_x -
                       StringWidth(temperature_text.str().c_str()) / 2.0,
                   temperature_y, temperature_text.str().c_str());
      }

      if (bar_index < this->visible_bars - 1) {
        const auto separator_x = (bar_index + 1) * this->adjusted_bar_width;
        DrawLine(separator_x, separator_start_y, separator_x, separator_stop_y,
                 LGRAY);
      }
    }

    const auto curve_y_offset = temperature_y - this->font->height;
    const auto curve_height =
        temperature_y - icon_y - this->font->height - 2 * PADDING;
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

    const auto width = ScreenWidth();
    for (int x_screen = 0; x_screen < width; ++x_screen) {
      const double x = this->forecast_offset * step + x_screen;
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

  void draw_middle_box_and_update() {
    this->draw_middle_box();

    const auto screen_height = ScreenHeight();
    const auto screen_width = ScreenWidth();
    PartialUpdate(0, this->top_box_height + 1, screen_width,
                  screen_height - this->status_bar_height);
  }

  void draw_status_bar() {
    const auto screen_height = ScreenHeight();
    const auto screen_width = ScreenWidth();
    const auto start_y = screen_height - this->status_bar_height;
    FillArea(0, start_y, screen_width, this->status_bar_height, 0x00FFFFFF);

    std::stringstream first_row_text;
    if (this->model->refresh_date == 0) {
      first_row_text << T("Ongoing update…");
    } else {
      std::string time_text{"?????"};
      const char *const time_format = "%H:%M";
      std::strftime(const_cast<char *>(time_text.c_str()), 6, time_format,
                    std::localtime(&this->model->refresh_date));
      first_row_text << T("Updated at") << " " << time_text << ".";
    }

    SetFont(this->tiny_font.get(), BLACK);
    const auto first_row_x = PADDING;
    const auto first_row_y = start_y + PADDING;
    DrawString(first_row_x, first_row_y, first_row_text.str().c_str());

    std::stringstream second_row_text;
    second_row_text << T("Weather data from") << " " << this->model->source;

    SetFont(this->tiny_font.get(), DGRAY);
    const auto second_row_x = PADDING;
    const auto second_row_y = first_row_y + GetFont()->height;
    DrawString(second_row_x, second_row_y, second_row_text.str().c_str());
  }
};

} // namespace taranis
