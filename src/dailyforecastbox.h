#pragma once

#pragma once

#include <algorithm>
#include <array>
#include <boost/variant.hpp>
#include <inkview.h>
#include <memory>
#include <numeric>

#include "events.h"
#include "fonts.h"
#include "icons.h"
#include "model.h"
#include "units.h"
#include "util.h"
#include "widget.h"

using namespace std::string_literals;

namespace taranis {

class DailyForecastBox : public Widget {
public:
  DailyForecastBox(int pos_x, int pos_y, int width, int height,
                   std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                   std::shared_ptr<Fonts> fonts)
      : Widget{pos_x, pos_y, width, height}, model{model}, icons{icons},
        fonts{fonts} {
    this->row_height = this->bounding_box.h / DailyForecastBox::row_count;
  }

  void do_paint() override {
    this->draw_values();
    this->draw_frame();
  }

private:
  static constexpr size_t row_count{8};
  static constexpr size_t column_count{9};
  static constexpr int horizontal_padding{25};

  typedef boost::variant<std::string, std::pair<std::string, std::string>,
                         ibitmap *>
      CellContent;
  // one text line, two text lines or an icon

  typedef std::array<CellContent, DailyForecastBox::row_count> ColumnContent;
  typedef std::array<ColumnContent, DailyForecastBox::column_count>
      TableContent;

  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  TableContent table_content;

  int row_height;

  std::array<size_t, DailyForecastBox::column_count> column_widths;
  std::array<size_t, DailyForecastBox::column_count> column_starts;

  static constexpr size_t WeekDayColumn{0};
  static constexpr size_t IconColumn{1};
  static constexpr size_t SunHoursColumn{2};
  static constexpr size_t MorningTemperatureColumn{3};
  static constexpr size_t DayTemperatureColumn{4};
  static constexpr size_t EveningTemperatureColumn{5};
  static constexpr size_t MinMaxTemperatureColumn{6};
  static constexpr size_t WindColumn{7};
  static constexpr size_t PrecipitationColumn{8};

  static constexpr std::array<int, DailyForecastBox::column_count>
      column_text_flags{ALIGN_LEFT,  ALIGN_CENTER, ALIGN_RIGHT,
                        ALIGN_RIGHT, ALIGN_RIGHT,  ALIGN_RIGHT,
                        ALIGN_RIGHT, ALIGN_RIGHT,  ALIGN_RIGHT};

  bool handle_key_press(int key) override {
    return (key == IV_KEY_PREV or key == IV_KEY_NEXT);
  }

  bool handle_key_release(int key) override {
    if (key == IV_KEY_PREV or key == IV_KEY_NEXT) {
      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM,
                CustomEvent::change_daily_forecast_display, 0);
      return true;
    }
    return false;
  }

  std::pair<std::string, std::string>
  generate_precipitation_column_content(const DailyCondition &forecast) const {
    const Units units{this->model};

    const auto precipitation = max_number(forecast.rain, forecast.snow);
    if (std::isnan(precipitation)) {
      return {"", ""};
    }
    const auto precipitation_text =
        units.format_precipitation(precipitation, false);

    const auto probability_of_precipitation =
        forecast.probability_of_precipitation;
    if (std::isnan(probability_of_precipitation)) {
      return {precipitation_text, ""};
    }
    const auto probability_of_precipitation_text =
        std::to_string(static_cast<int>(probability_of_precipitation * 100)) +
        "%";
    return {precipitation_text, probability_of_precipitation_text};
  }

  void generate_table_content() {
    const Units units{this->model};

    for (size_t column_index = 0; column_index < DailyForecastBox::column_count;
         ++column_index) {
      auto &column_content = this->table_content.at(column_index);

      for (size_t row_index = 0; row_index < DailyForecastBox::row_count;
           ++row_index) {
        if (row_index < this->model->daily_forecast.size()) {
          const auto &forecast = this->model->daily_forecast[row_index];

          if (column_index == DailyForecastBox::WeekDayColumn) {
            column_content[row_index] = std::pair<std::string, std::string>{
                format_day(forecast.date), format_short_date(forecast.date)};
          } else if (column_index == DailyForecastBox::IconColumn) {
            column_content[row_index] =
                this->icons->get(forecast.weather_icon_name);
          } else if (column_index == DailyForecastBox::SunHoursColumn) {
            column_content[row_index] = std::pair<std::string, std::string>{
                format_time(forecast.sunset), format_time(forecast.sunrise)};
          } else if (column_index ==
                     DailyForecastBox::MorningTemperatureColumn) {
            column_content[row_index] =
                units.format_temperature(forecast.temperature_morning);
          } else if (column_index == DailyForecastBox::DayTemperatureColumn) {
            column_content[row_index] =
                units.format_temperature(forecast.temperature_day);
          } else if (column_index ==
                     DailyForecastBox::EveningTemperatureColumn) {
            column_content[row_index] =
                units.format_temperature(forecast.temperature_evening);
          } else if (column_index ==
                     DailyForecastBox::MinMaxTemperatureColumn) {
            column_content[row_index] = std::pair<std::string, std::string>{
                units.format_temperature(forecast.temperature_max),
                units.format_temperature(forecast.temperature_min)};
          } else if (column_index == DailyForecastBox::WindColumn) {
            column_content[row_index] = std::pair<std::string, std::string>{
                units.format_speed(forecast.wind_speed),
                std::to_string(static_cast<int>(forecast.humidity)) + "%"};
          } else if (column_index == DailyForecastBox::PrecipitationColumn) {
            column_content[row_index] =
                this->generate_precipitation_column_content(forecast);
          }
        } else {
          if (column_index == DailyForecastBox::IconColumn) {
            column_content[row_index] = static_cast<ibitmap *>(nullptr);
          } else {
            column_content[row_index] = "";
          }
        }
      }
    }
  }

  std::shared_ptr<ifont> get_column_first_line_font(size_t column_index) const {
    if (column_index == DailyForecastBox::WeekDayColumn) {
      return this->fonts->get_small_font();
    } else if (column_index == DailyForecastBox::SunHoursColumn or
               column_index == DailyForecastBox::MinMaxTemperatureColumn or
               column_index == DailyForecastBox::WindColumn or
               column_index == DailyForecastBox::PrecipitationColumn) {
      return this->fonts->get_tiny_font();
    } else if (column_index == DailyForecastBox::MorningTemperatureColumn or
               column_index == DailyForecastBox::DayTemperatureColumn or
               column_index == DailyForecastBox::EveningTemperatureColumn) {
      return this->fonts->get_small_bold_font();
    }
    return nullptr;
  }

  int estimate_max_content_width(size_t column_index) const {
    if (column_index == DailyForecastBox::IconColumn) {
      return 70;
    }
    if (not(0 <= column_index and
            column_index < DailyForecastBox::column_count)) {
      return 0;
    }
    const auto first_line_font = this->get_column_first_line_font(column_index);
    if (not first_line_font) {
      return 0;
    }
    SetFont(first_line_font.get(), BLACK);

    const auto second_line_font = this->fonts->get_tiny_font();

    auto &column_content = this->table_content.at(column_index);
    std::array<int, DailyForecastBox::row_count> widths;
    std::transform(
        std::begin(column_content), std::end(column_content),
        std::begin(widths),
        [first_line_font, second_line_font](const CellContent &content) {
          if (content.type() == typeid(std::string)) {
            return StringWidth(boost::get<std::string>(content).c_str());
          }
          const auto &values =
              boost::get<std::pair<std::string, std::string>>(content);

          const auto first_line_text_width = StringWidth(values.first.c_str());
          SetFont(second_line_font.get(), BLACK);
          const auto second_line_text_width =
              StringWidth(values.second.c_str());
          SetFont(first_line_font.get(), BLACK);

          return std::max(first_line_text_width, second_line_text_width);
        });
    const auto &max_width =
        std::max_element(std::begin(widths), std::end(widths));
    return max_width == std::end(widths) ? 0 : *max_width;
  }

  void compute_columns_layout() {
    for (size_t column_index = 0; column_index < DailyForecastBox::column_count;
         ++column_index) {
      this->column_widths[column_index] =
          this->estimate_max_content_width(column_index);
    }
    const auto total_content_width = std::accumulate(
        std::begin(this->column_widths), std::end(this->column_widths), 0);
    const auto remaining_pixels =
        std::max(ScreenWidth() - 2 * DailyForecastBox::horizontal_padding -
                     total_content_width,
                 0);
    const auto to_add = remaining_pixels / DailyForecastBox::column_count;

    size_t column_index = 0;
    this->column_widths[0] += to_add;
    this->column_starts[0] =
        this->bounding_box.x + DailyForecastBox::horizontal_padding;
    ++column_index;
    while (column_index < DailyForecastBox::column_count) {
      const auto previous_column_index = column_index - 1;
      this->column_widths[column_index] += to_add;
      this->column_starts[column_index] =
          this->column_starts[previous_column_index] +
          this->column_widths[previous_column_index];
      ++column_index;
    }
  }

  void draw_frame() const {
    const auto separator_start_x = this->bounding_box.x;
    const auto separator_stop_x = this->bounding_box.w;

    for (size_t row_index = 0; row_index <= DailyForecastBox::row_count;
         ++row_index) {
      const auto separator_start_y =
          (row_index < DailyForecastBox::row_count
               ? this->bounding_box.y + row_index * this->row_height
               : this->bounding_box.y + this->bounding_box.h);
      const auto separator_stop_y = separator_start_y;

      DrawLine(separator_start_x, separator_start_y, separator_stop_x,
               separator_stop_y, LGRAY);
    }
  }

  void draw_values() {
    this->generate_table_content();
    this->compute_columns_layout();

    const auto second_line_font = this->fonts->get_tiny_font();

    for (size_t column_index = 0; column_index < DailyForecastBox::column_count;
         ++column_index) {
      const auto &column_content = this->table_content[column_index];
      const auto &column_start_x = this->column_starts[column_index];

      if (column_index == DailyForecastBox::IconColumn) {
        int row_start_y = this->bounding_box.y;
        for (size_t row_index = 0; row_index < DailyForecastBox::row_count;
             ++row_index) {
          const auto icon = boost::get<ibitmap *>(column_content[row_index]);
          if (icon) {
            DrawBitmap(column_start_x, row_start_y, icon);
          }
          row_start_y += this->row_height;
        }
      } else {
        const auto &column_width = this->column_widths[column_index];
        const auto &text_flags = this->column_text_flags[column_index];
        const auto first_line_font =
            this->get_column_first_line_font(column_index);
        SetFont(first_line_font.get(), BLACK);

        int row_start_y = this->bounding_box.y;
        for (size_t row_index = 0; row_index < DailyForecastBox::row_count;
             ++row_index) {
          const auto &cell_value = column_content[row_index];
          if (cell_value.type() == typeid(std::string)) {
            const auto &text =
                boost::get<std::string>(column_content[row_index]);
            DrawTextRect(column_start_x, row_start_y + 1, column_width,
                         this->row_height - 2, text.c_str(),
                         text_flags | VALIGN_MIDDLE);
          } else {
            const auto &content =
                boost::get<std::pair<std::string, std::string>>(
                    column_content[row_index]);
            const auto &first_line_text = content.first;
            const auto &second_line_text = content.second;

            const auto &line_height = (row_height - 2) / 2;
            DrawTextRect(column_start_x, row_start_y + 1, column_width,
                         line_height, first_line_text.c_str(),
                         text_flags | VALIGN_MIDDLE);

            SetFont(second_line_font.get(), BLACK);

            DrawTextRect(column_start_x, row_start_y + 1 + line_height,
                         column_width, line_height, second_line_text.c_str(),
                         text_flags | VALIGN_MIDDLE);

            SetFont(first_line_font.get(), BLACK);
          }
          row_start_y += this->row_height;
        }
      }
    }
  }
};

} // namespace taranis
