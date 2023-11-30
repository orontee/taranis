#include "dailyforecastbox.h"

#include "units.h"
#include "util.h"

namespace taranis {
DailyForecastBox::DailyForecastBox(int pos_x, int pos_y, int width, int height,
                                   std::shared_ptr<Model> model,
                                   std::shared_ptr<Icons> icons,
                                   std::shared_ptr<Fonts> fonts)
    : Widget{pos_x, pos_y, width, height}, model{model}, icons{icons},
      fonts{fonts} {
  this->row_height = this->bounding_box.h / DailyForecastBox::row_count;
}

void DailyForecastBox::do_paint() {
  this->draw_values();
  this->draw_frame();
}

std::pair<std::string, std::string>
DailyForecastBox::generate_precipitation_column_content(
    const DailyCondition &forecast) const {
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

void DailyForecastBox::generate_table_content() {
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
              format_short_day(forecast.date),
              format_short_date(forecast.date)};
        } else if (column_index == DailyForecastBox::IconColumn) {
          column_content[row_index] =
              this->icons->get(forecast.weather_icon_name);
        } else if (column_index == DailyForecastBox::SunHoursColumn) {
          column_content[row_index] = std::pair<std::string, std::string>{
              format_time(forecast.sunset), format_time(forecast.sunrise)};
        } else if (column_index == DailyForecastBox::MorningTemperatureColumn) {
          column_content[row_index] =
              units.format_temperature(forecast.temperature_morning);
        } else if (column_index == DailyForecastBox::DayTemperatureColumn) {
          column_content[row_index] =
              units.format_temperature(forecast.temperature_day);
        } else if (column_index == DailyForecastBox::EveningTemperatureColumn) {
          column_content[row_index] =
              units.format_temperature(forecast.temperature_evening);
        } else if (column_index == DailyForecastBox::MinMaxTemperatureColumn) {
          column_content[row_index] = std::pair<std::string, std::string>{
              units.format_temperature(forecast.temperature_max),
              units.format_temperature(forecast.temperature_min)};
        } else if (column_index == DailyForecastBox::WindColumn) {
          const auto wind_speed_text = units.format_speed(forecast.wind_speed);
          const auto wind_gust_text = units.format_speed(forecast.wind_gust);
          if (forecast.wind_gust > forecast.wind_speed and
              wind_gust_text != wind_speed_text) {
            column_content[row_index] = std::pair<std::string, std::string>{
                wind_speed_text, wind_gust_text};
          } else {
            column_content[row_index] =
                std::pair<std::string, std::string>{wind_speed_text, ""};
          }
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

std::shared_ptr<ifont> DailyForecastBox::get_font(size_t column_index,
                                                  int line_number) const {
  if (column_index == DailyForecastBox::WeekDayColumn) {
    return this->fonts->get_small_font();
  } else if (column_index == DailyForecastBox::SunHoursColumn or
             column_index == DailyForecastBox::MinMaxTemperatureColumn or
             column_index == DailyForecastBox::PrecipitationColumn) {
    return this->fonts->get_tiny_font();
  } else if (column_index == DailyForecastBox::MorningTemperatureColumn or
             column_index == DailyForecastBox::DayTemperatureColumn or
             column_index == DailyForecastBox::EveningTemperatureColumn) {
    return this->fonts->get_small_bold_font();
  } else if (column_index == DailyForecastBox::WindColumn) {
    if (line_number == 2) {
      return this->fonts->get_tiny_italic_font();
    } else {
      return this->fonts->get_tiny_font();
    }
  }
  return nullptr;
}

int DailyForecastBox::estimate_max_content_width(size_t column_index) const {
  if (column_index == DailyForecastBox::IconColumn) {
    return 70;
  }
  if (not(0 <= column_index and
          column_index < DailyForecastBox::column_count)) {
    return 0;
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

  for (size_t column_index = 0; column_index < DailyForecastBox::column_count;
       ++column_index) {
    const auto &column_content = this->table_content[column_index];
    const auto &column_start_x = this->column_starts[column_index];

    if (column_index == DailyForecastBox::IconColumn) {
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
      const auto &text_flags = this->column_text_flags[column_index];
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
} // namespace taranis
