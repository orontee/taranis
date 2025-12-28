#include "dailyforecastbox.h"

#include <algorithm>
#include <experimental/optional>

#include "dailyforecastviewer.h"
#include "inkview.h"
#include "units.h"
#include "util.h"

namespace taranis {
DailyForecastBox::DailyForecastBox(
    int pos_x, int pos_y, int width, int height, std::shared_ptr<Model> model,
    std::shared_ptr<Icons> icons, std::shared_ptr<Fonts> fonts,
    std::shared_ptr<DailyForecastViewer> daily_forecast_viewer)
    : Widget{pos_x, pos_y, width, height}, model{model}, icons{icons},
      fonts{fonts}, viewer{daily_forecast_viewer} {
  this->row_height = this->bounding_box.h / DailyForecastBox::row_count;
}

void DailyForecastBox::do_paint() {
  this->draw_values();
  this->draw_frame();
}

int DailyForecastBox::handle_pointer_event(int event_type, int pointer_pos_x,
                                           int pointer_pos_y) {
  // check pos on frame, identify forecast index, set viewer forecast index
  if (event_type == EVT_POINTERUP) {
    this->on_clicked_at(pointer_pos_x, pointer_pos_y);
    return 1;
  }
  return 0;
}

std::pair<std::string, std::string>
DailyForecastBox::generate_precipitation_column_content(
    const DailyCondition &forecast) const {
  const Units units{this->model};

  const auto precipitation = max_number(forecast.rain, forecast.snow);
  if (std::isnan(precipitation) or precipitation == 0) {
    return {"", ""};
  }
  const auto precipitation_text =
      units.format_precipitation(precipitation, false);

  const auto probability_of_precipitation =
      forecast.probability_of_precipitation;
  if (std::isnan(probability_of_precipitation) or probability_of_precipitation == 0) {
    return {precipitation_text, ""};
  }
  const auto probability_of_precipitation_text =
      std::to_string(static_cast<int>(probability_of_precipitation * 100)) +
      "%";
  return {precipitation_text, probability_of_precipitation_text};
}

void DailyForecastBox::generate_table_content() {
  const bool has_day_periods_temperatures = std::all_of(
      std::begin(this->model->daily_forecast),
      std::end(this->model->daily_forecast), [](const auto &forecast) {
        return (forecast.temperature_morning and
                forecast.temperature_evening and forecast.temperature_night);
      });
  if (has_day_periods_temperatures) {
    this->column_count = 9;
    this->column_types = {WeekDayColumn,           IconColumn,
                          SunHoursColumn,          MorningTemperatureColumn,
                          DayTemperatureColumn,    EveningTemperatureColumn,
                          MinMaxTemperatureColumn, WindColumn,
                          PrecipitationColumn};
  } else {
    this->column_count = 8;
    this->column_types = {WeekDayColumn,
                          IconColumn,
                          SunHoursColumn,
                          DayTemperatureColumn,
                          DayFeltTemperatureColumn,
                          MinMaxTemperatureColumn,
                          WindColumn,
                          PrecipitationColumn};
  }
  this->table_content.clear();
  this->column_widths.clear();
  this->column_starts.clear();

  this->table_content.resize(this->column_count);
  this->column_widths.resize(this->column_count);
  this->column_starts.resize(this->column_count);

  const Units units{this->model};

  for (size_t column_index = 0; column_index < this->column_count;
       ++column_index) {
    auto &column_content = this->table_content.at(column_index);
    const auto column_type = this->column_types.at(column_index);

    for (size_t row_index = 0; row_index < DailyForecastBox::row_count;
         ++row_index) {
      if (row_index < this->model->daily_forecast.size()) {
        const auto &forecast = this->model->daily_forecast[row_index];

        if (column_type == DailyForecastBox::WeekDayColumn) {
          column_content[row_index] = std::pair<std::string, std::string>{
              format_short_day(forecast.date),
              format_short_date(forecast.date)};
        } else if (column_type == DailyForecastBox::IconColumn) {
          column_content[row_index] =
              this->icons->get(forecast.weather_icon_name);
        } else if (column_type == DailyForecastBox::SunHoursColumn) {
          column_content[row_index] = std::pair<std::string, std::string>{
              format_time(forecast.sunrise), format_time(forecast.sunset)};
        } else if (column_type == DailyForecastBox::MorningTemperatureColumn) {
          if (forecast.temperature_morning != std::experimental::nullopt) {
            column_content[row_index] =
                units.format_temperature(*forecast.temperature_morning);
          }
        } else if (column_type == DailyForecastBox::DayTemperatureColumn) {
          column_content[row_index] =
              units.format_temperature(forecast.temperature_day);
        } else if (column_type == DailyForecastBox::DayFeltTemperatureColumn) {
          column_content[row_index] =
              units.format_temperature(forecast.felt_temperature_day);
        } else if (column_type == DailyForecastBox::EveningTemperatureColumn) {
          if (forecast.temperature_evening != std::experimental::nullopt) {
            column_content[row_index] =
                units.format_temperature(*forecast.temperature_evening);
          }
        } else if (column_type == DailyForecastBox::MinMaxTemperatureColumn) {
          column_content[row_index] = std::pair<std::string, std::string>{
              units.format_temperature(forecast.temperature_max),
              units.format_temperature(forecast.temperature_min)};
        } else if (column_type == DailyForecastBox::WindColumn) {
          if (static_cast<int>(forecast.wind_speed) == 0 and
              static_cast<int>(forecast.wind_gust) == 0) {
            column_content[row_index] =
                std::pair<std::string, std::string>{"", ""};
          } else {
            const auto wind_speed_text =
                units.format_speed(forecast.wind_speed);
            const auto wind_gust_text = units.format_speed(forecast.wind_gust);
            if (static_cast<int>(forecast.wind_speed) == 0) {
              column_content[row_index] =
                  std::pair<std::string, std::string>{"", wind_gust_text};
            } else if (forecast.wind_gust > forecast.wind_speed and
                       wind_gust_text != wind_speed_text) {
              column_content[row_index] = std::pair<std::string, std::string>{
                  wind_speed_text, wind_gust_text};
            } else {
              column_content[row_index] =
                  std::pair<std::string, std::string>{wind_speed_text, ""};
            }
          }
        } else if (column_type == DailyForecastBox::PrecipitationColumn) {
          column_content[row_index] =
              this->generate_precipitation_column_content(forecast);
        }
      } else {
        if (column_type == DailyForecastBox::IconColumn) {
          column_content[row_index] = static_cast<ibitmap *>(nullptr);
        } else {
          column_content[row_index] = "";
        }
      }
    }
  }
}

std::shared_ptr<ifont> DailyForecastBox::get_font(size_t column_index,
                                                  int line_number) const {
  assert(0 <= column_index and column_index < this->column_types.size());
  const auto column_type = this->column_types.at(column_index);
  if (column_type == WeekDayColumn) {
    if (line_number == 2) {
      return this->fonts->get_tiny_font();
    } else {
      return this->fonts->get_small_font();
    }
  } else if (column_type == SunHoursColumn or
             column_type == MinMaxTemperatureColumn or
             column_type == PrecipitationColumn) {
    return this->fonts->get_tiny_font();
  } else if (column_type == MorningTemperatureColumn or
             column_type == DayTemperatureColumn or
             column_type == EveningTemperatureColumn) {
    return this->fonts->get_small_bold_font();
  } else if (column_type == DayFeltTemperatureColumn) {
    return this->fonts->get_small_font();
  } else if (column_type == WindColumn) {
    if (line_number == 2) {
      return this->fonts->get_tiny_italic_font();
    } else {
      return this->fonts->get_tiny_font();
    }
  }
  return nullptr;
}

int DailyForecastBox::estimate_max_content_width(size_t column_index) const {
  if (not(0 <= column_index and column_index < this->column_count)) {
    return 0;
  }
  const auto column_type = this->column_types.at(column_index);
  if (column_type == DailyForecastBox::IconColumn) {
    return 70;
  }
  const auto first_line_font = this->get_font(column_index, 1);
  if (not first_line_font) {
    return 0;
  }
  SetFont(first_line_font.get(), BLACK);

  const auto second_line_font = this->get_font(column_index, 2);

  auto &column_content = this->table_content.at(column_index);
  std::array<int, DailyForecastBox::row_count> widths;
  std::transform(
      std::begin(column_content), std::end(column_content), std::begin(widths),
      [first_line_font, second_line_font](const CellContent &content) {
        if (content.type() == typeid(std::string)) {
          return StringWidth(boost::get<std::string>(content).c_str());
        }
        const auto &values =
            boost::get<std::pair<std::string, std::string>>(content);

        const auto first_line_text_width = StringWidth(values.first.c_str());
        SetFont(second_line_font.get(), BLACK);
        const auto second_line_text_width = StringWidth(values.second.c_str());
        SetFont(first_line_font.get(), BLACK);

        return std::max(first_line_text_width, second_line_text_width);
      });
  const auto &max_width =
      std::max_element(std::begin(widths), std::end(widths));
  return max_width == std::end(widths) ? 0 : *max_width;
}

void DailyForecastBox::compute_columns_layout() {
  for (size_t column_index = 0; column_index < this->column_count;
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
  const auto to_add = remaining_pixels / this->column_count;

  size_t column_index = 0;
  this->column_widths[0] += to_add;
  this->column_starts[0] =
      this->bounding_box.x + DailyForecastBox::horizontal_padding;
  ++column_index;
  while (column_index < this->column_count) {
    const auto previous_column_index = column_index - 1;
    this->column_widths[column_index] += to_add;
    this->column_starts[column_index] =
        this->column_starts[previous_column_index] +
        this->column_widths[previous_column_index];
    ++column_index;
  }
}

void DailyForecastBox::draw_frame() const {
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

void DailyForecastBox::draw_values() {
  this->generate_table_content();
  this->compute_columns_layout();

  for (size_t column_index = 0; column_index < this->column_count;
       ++column_index) {
    const auto &column_content = this->table_content[column_index];
    const auto &column_start_x = this->column_starts[column_index];
    const auto column_type = this->column_types[column_index];

    if (column_type == DailyForecastBox::IconColumn) {
      for (size_t row_index = 0; row_index < DailyForecastBox::row_count;
           ++row_index) {
        const auto icon = boost::get<ibitmap *>(column_content[row_index]);
        if (icon) {
          const int icon_offset_y = (this->row_height - icon->height) / 2;
          const int icon_start_y = this->bounding_box.y +
                                   row_index * this->row_height + icon_offset_y;
          DrawBitmap(column_start_x, icon_start_y, icon);
        }
      }
    } else {
      const auto &column_width = this->column_widths[column_index];
      const auto text_flags = this->column_text_flags(column_type);
      const auto first_line_font = this->get_font(column_index, 1);
      const auto second_line_font = this->get_font(column_index, 2);

      for (size_t row_index = 0; row_index < DailyForecastBox::row_count;
           ++row_index) {
        const int row_start_y =
            this->bounding_box.y + row_index * this->row_height;
        const auto &cell_value = column_content[row_index];
        if (cell_value.type() == typeid(std::string)) {
          SetFont(first_line_font.get(), BLACK);

          const auto &text = boost::get<std::string>(column_content[row_index]);
          DrawTextRect(column_start_x, row_start_y + 1, column_width,
                       this->row_height - 2, text.c_str(),
                       text_flags | VALIGN_MIDDLE);
        } else {
          const auto &content = boost::get<std::pair<std::string, std::string>>(
              column_content[row_index]);
          const auto &first_line_text = content.first;
          const auto &second_line_text = content.second;

          SetFont(first_line_font.get(), BLACK);

          const auto &line_height = (row_height - 2) / 2;
          DrawTextRect(column_start_x, row_start_y + 1, column_width,
                       line_height, first_line_text.c_str(),
                       text_flags | VALIGN_MIDDLE);

          SetFont(second_line_font.get(), BLACK);

          DrawTextRect(column_start_x, row_start_y + 1 + line_height,
                       column_width, line_height, second_line_text.c_str(),
                       text_flags | VALIGN_MIDDLE);
        }
      }
    }
  }
}

void DailyForecastBox::on_clicked_at(int pointer_pos_x, int pointer_pos_y) {
  if (this->viewer) {
    const auto row_index = static_cast<size_t>(
        std::max(0, pointer_pos_y - this->bounding_box.y) / this->row_height);
    this->viewer->set_forecast_index(row_index);
    this->viewer->open();
  }
}

int DailyForecastBox::column_text_flags(ColumnType type) {
  switch (type) {
  case WeekDayColumn:
    return ALIGN_LEFT;
  case IconColumn:
    return ALIGN_CENTER;
  case SunHoursColumn:
  case MorningTemperatureColumn:
  case DayTemperatureColumn:
  case DayFeltTemperatureColumn:
  case EveningTemperatureColumn:
  case MinMaxTemperatureColumn:
  case WindColumn:
  case PrecipitationColumn:
    return ALIGN_RIGHT;
  }
  return -1;
}
} // namespace taranis
