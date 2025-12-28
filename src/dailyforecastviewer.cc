#include "dailyforecastviewer.h"

#include <cstring>
#include <experimental/optional>
#include <inkview.h>

#include "units.h"
#include "util.h"

namespace taranis {

const RowDescription empty_row{"", "", false, "", true};

DailyForecastViewer::DailyForecastViewer(int pos_y,
                                         std::shared_ptr<Model> model,
                                         std::shared_ptr<Icons> icons,
                                         std::shared_ptr<Fonts> fonts)
    : ModalWidget{DailyForecastViewer::horizontal_padding, pos_y,
                  ScreenWidth() - 2 * DailyForecastViewer::horizontal_padding,
                  ScreenHeight() - pos_y -
                      DailyForecastViewer::vertical_padding},
      model{model}, icons{icons}, fonts{fonts} {
  this->swipe_detector.set_bounding_box(this->bounding_box);
}

void DailyForecastViewer::open() {
  if (this->forecast_index >= this->model->daily_forecast.size()) {
    BOOST_LOG_TRIVIAL(warning)
        << "Won't open forecast viewer, due to invalid forecast index";
    return;
  }
  this->visible = true;

  const auto &condition = this->model->daily_forecast.at(this->forecast_index);
  this->generate_description_data(condition);

  this->identify_scrollable_area();
  AddScrolledArea(&this->scrollable_view_rectangle, true);

  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_SHOW, 0, 0);
}

void DailyForecastViewer::hide() {
  this->visible = false;
  RemoveScrolledArea(&this->scrollable_view_rectangle);

  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_SHOW, 0, 0);
}

void DailyForecastViewer::set_forecast_index(size_t index) {
  if (index >= this->model->daily_forecast.size()) {
    BOOST_LOG_TRIVIAL(warning) << "Attempt to set an invalid forecast index";
    return;
  }
  this->forecast_index = index;
}

void DailyForecastViewer::do_paint() {
  const auto default_font = this->fonts->get_small_font();
  const auto bold_font = this->fonts->get_small_bold_font();
  const auto tiny_font = this->fonts->get_tiny_font();

  if (this->forecast_index >= this->model->daily_forecast.size()) {
    BOOST_LOG_TRIVIAL(warning) << "Invalid forecast index";
    return;
  }
  const auto &condition = this->model->daily_forecast.at(this->forecast_index);

  // title
  SetFont(bold_font.get(), BLACK);

  const auto title_height =
      bold_font->height + 2 * DailyForecastViewer::vertical_padding;
  const auto title_text = format_date(condition.date, true);

  DrawTextRect(this->get_pos_x() + DailyForecastViewer::horizontal_padding,
               this->get_pos_y(),
               this->get_width() - 2 * DailyForecastViewer::horizontal_padding,
               title_height, title_text.c_str(), ALIGN_LEFT | VALIGN_MIDDLE);

  if (!this->close_button) {
    const auto close_button_icon_size =
        bold_font->height + DailyForecastViewer::vertical_padding;

    this->close_button =
        std::make_shared<Button>(close_button_icon_size, "close", this->icons);
    this->close_button->set_click_handler(
        std::bind(&DailyForecastViewer::hide, this));
    this->close_button->set_pos_x(this->get_width() - close_button_icon_size -
                                  DailyForecastViewer::horizontal_padding);
    this->close_button->set_pos_y(this->get_pos_y() +
                                  DailyForecastViewer::vertical_padding / 4);
  }
  this->close_button->do_paint();

  DrawHorizontalSeparator(
      this->get_pos_x() + DailyForecastViewer::horizontal_padding,
      this->get_pos_y() + title_height,
      this->get_width() - 2 * DailyForecastViewer::horizontal_padding,
      HORIZONTAL_SEPARATOR_SOLID);

  // forecast
  SetFont(default_font.get(), BLACK);

  SetClipRect(&this->scrollable_view_rectangle);

  int current_row_start_y = this->scrollable_view_rectangle.y +
                            DailyForecastViewer::vertical_padding +
                            this->scrollable_view_offset;
  const int two_values_column_width = this->scrollable_view_rectangle.w / 4;
  const int first_value_column_start_x =
      this->scrollable_view_rectangle.x + this->scrollable_view_rectangle.w / 2;
  const int four_values_column_width = this->scrollable_view_rectangle.w / 8;

  for (auto const &row_description : this->description_data) {
    const auto &row_value_type = row_description.value.type();
    // better use std::holds_alternative but no equivalent in boost
    // implementation of variant...

    if (row_description.is_empty) {
      current_row_start_y += bold_font->height;
    } else {

      // render label with optional icon

      const auto font = row_description.is_header ? bold_font : default_font;
      SetFont(font.get(), BLACK);

      const auto label_text = row_description.label;

      const auto icon_name = row_description.icon_name;
      const auto icon_size = std::max(
          4 * DailyForecastViewer::horizontal_padding, default_font->height);

      const auto &icon_data = this->icons->rotate_icon(icon_name, icon_size, 0);
      if (not icon_data.empty()) {

        const auto icon = reinterpret_cast<const ibitmap *>(icon_data.data());

        DrawBitmap(this->get_pos_x() + DailyForecastViewer::horizontal_padding,
                   current_row_start_y, icon);
      }

      const auto label_start_x = this->get_pos_x() +
                                 DailyForecastViewer::horizontal_padding +
                                 (not icon_data.empty() ? icon_size : 0);
      const auto row_start_y =
          current_row_start_y + (not icon_data.empty() ? icon_size / 4 : 0);
      const auto label_width = StringWidth(label_text.c_str());
      DrawTextRect(label_start_x, row_start_y, label_width, font.get()->height,
                   label_text.c_str(), ALIGN_LEFT);

      // render a single value (string or icon), two or four values

      if (row_value_type == typeid(std::string)) {

        SetFont(default_font.get(), BLACK);

        const auto first_text = boost::get<std::string>(row_description.value);

        DrawTextRect(
            this->get_pos_x() + DailyForecastViewer::horizontal_padding +
                label_width,
            row_start_y, this->scrollable_view_rectangle.w - label_width,
            default_font.get()->height, first_text.c_str(), ALIGN_RIGHT);

      } else if (row_value_type ==
                 typeid(std::pair<std::string, std::string>)) {

        const auto font = row_description.is_header ? tiny_font : default_font;
        SetFont(font.get(), BLACK);

        std::string first_text, second_text;
        std::tie(first_text, second_text) =
            boost::get<std::pair<std::string, std::string>>(
                row_description.value);

        DrawTextRect(first_value_column_start_x, current_row_start_y,
                     two_values_column_width, font.get()->height,
                     first_text.c_str(), ALIGN_RIGHT);

        const int second_value_column_start_x =
            first_value_column_start_x + two_values_column_width;

        DrawTextRect(second_value_column_start_x, current_row_start_y,
                     two_values_column_width, font.get()->height,
                     second_text.c_str(), ALIGN_RIGHT);

      } else if (row_value_type ==
                 typeid(std::tuple<std::string, std::string, std::string,
                                   std::string>)) {

        const auto font = row_description.is_header ? tiny_font : default_font;
        SetFont(font.get(), BLACK);

        const auto value_alignment = ALIGN_RIGHT;

        std::string first_text, second_text, third_text, fourth_text;
        std::tie(first_text, second_text, third_text, fourth_text) = boost::get<
            std::tuple<std::string, std::string, std::string, std::string>>(
            row_description.value);

        DrawTextRect(first_value_column_start_x, current_row_start_y,
                     four_values_column_width, default_font.get()->height,
                     first_text.c_str(), value_alignment);

        const int second_value_column_start_x =
            first_value_column_start_x + four_values_column_width;

        DrawTextRect(second_value_column_start_x, current_row_start_y,
                     four_values_column_width, default_font.get()->height,
                     second_text.c_str(), value_alignment);

        const int third_value_column_start_x =
            first_value_column_start_x + 2 * four_values_column_width;

        DrawTextRect(third_value_column_start_x, current_row_start_y,
                     four_values_column_width, default_font.get()->height,
                     third_text.c_str(), value_alignment);

        const int fourth_value_column_start_x =
            first_value_column_start_x + 3 * four_values_column_width;

        DrawTextRect(fourth_value_column_start_x, current_row_start_y,
                     four_values_column_width, default_font.get()->height,
                     fourth_text.c_str(), value_alignment);

      } else if (row_value_type == typeid(std::pair<std::string, int>)) {

        SetFont(default_font.get(), BLACK);

        std::string icon_name;
        int degree;
        std::tie(icon_name, degree) =
            boost::get<std::pair<std::string, int>>(row_description.value);

        const auto &rotated_icon_data =
            this->icons->rotate_icon(icon_name, default_font->size, degree);
        if (not rotated_icon_data.empty()) {

          const auto icon =
              reinterpret_cast<const ibitmap *>(rotated_icon_data.data());

          DrawBitmap(this->get_pos_x() + this->get_width() -
                         DailyForecastViewer::horizontal_padding - icon->width,
                     current_row_start_y, icon);
        }
      }
      current_row_start_y +=
          (not icon_data.empty() ? icon_size : default_font->height);
    }
  }
  SetClip(0, 0, ScreenWidth(), ScreenHeight());

  DrawFrameRectCertified(this->bounding_box, THICKNESS_DEF_FOCUSED_FRAME);
}

void DailyForecastViewer::identify_scrollable_area() {
  const auto default_font = this->fonts->get_small_font();
  const auto bold_font = this->fonts->get_small_bold_font();

  const auto scrollable_view_start_x =
      this->get_pos_x() + DailyForecastViewer::horizontal_padding;
  const auto title_height =
      bold_font->height + 2 * DailyForecastViewer::vertical_padding;
  const auto scrollable_view_start_y = this->get_pos_y() + title_height;

  this->scrollable_view_rectangle =
      iRect(scrollable_view_start_x, scrollable_view_start_y,
            this->get_width() - 2 * DailyForecastViewer::horizontal_padding,
            this->get_height() - title_height, 0);

  this->scrollable_view_offset = 0;

  const int description_height =
      this->description_data.size() * default_font->height +
      default_font->height + 2 * DailyForecastViewer::vertical_padding;

  this->min_scrollable_view_offset =
      -std::max(0, description_height - this->scrollable_view_rectangle.h);
}

void DailyForecastViewer::generate_description_data(
    const DailyCondition &condition) {
  this->description_data.clear();

  const Units units{this->model};

  std::string description_text = condition.weather_description;
  if (not description_text.empty()) {
    description_text[0] = std::toupper(description_text[0]);
  }
  this->description_data.push_back(
      RowDescription{description_text, "", false, condition.weather_icon_name});

  if (std::strcmp(currentLang(), "en") == 0) {
    // summary isn't translated right now
    if (not condition.summary.empty()) {
      this->description_data.push_back(RowDescription{condition.summary});
    }
  }
  if (not condition.weather_secondary_description.empty()) {
    this->description_data.push_back(
        RowDescription{condition.weather_secondary_description});
  }

  this->description_data.push_back(empty_row);

  const bool with_moon_data =
      (condition.moonrise != std::experimental::nullopt and
       condition.moonset != std::experimental::nullopt and
       condition.moon_phase != std::experimental::nullopt);

  if (with_moon_data) {
    this->description_data.push_back(RowDescription{
        "", std::make_pair(GetLangText("Sun"), GetLangText("Moon")), true});

    this->description_data.push_back(
        RowDescription{std::string{GetLangText("Rise")},
                       std::make_pair(format_time(condition.sunrise),
                                      format_time(*condition.moonrise))});

    this->description_data.push_back(
        RowDescription{std::string{GetLangText("Set")},
                       std::make_pair(format_time(condition.sunset),
                                      format_time(*condition.moonset))});

    this->description_data.push_back(
        RowDescription{std::string{GetLangText("Moon phase")},
                       format_moon_phase(*condition.moon_phase)});
  } else {
    this->description_data.push_back(
        RowDescription{GetLangText("Sun"), "", true});

    this->description_data.push_back(RowDescription{
        std::string{GetLangText("Rise")}, format_time(condition.sunrise)});

    this->description_data.push_back(RowDescription{
        std::string{GetLangText("Set")}, format_time(condition.sunset)});
  }
  this->description_data.push_back(empty_row);

  const bool with_multiple_temperature_data =
      (condition.temperature_morning != std::experimental::nullopt and
       condition.temperature_evening != std::experimental::nullopt and
       condition.temperature_night != std::experimental::nullopt and
       condition.felt_temperature_morning != std::experimental::nullopt and
       condition.felt_temperature_evening != std::experimental::nullopt and
       condition.felt_temperature_night != std::experimental::nullopt);
  if (with_multiple_temperature_data) {
    this->description_data.push_back(RowDescription{
        GetLangText("Temperatures"),
        std::make_tuple(GetLangText("Morning"), GetLangText("Day"),
                        GetLangText("Evening"), GetLangText("Night")),
        true});

    this->description_data.push_back(RowDescription{
        "", std::make_tuple(
                units.format_temperature(*condition.temperature_morning),
                units.format_temperature(condition.temperature_day),
                units.format_temperature(*condition.temperature_evening),
                units.format_temperature(*condition.temperature_night))});

    this->description_data.push_back(RowDescription{
        GetLangText("Felt"),
        std::make_tuple(
            units.format_temperature(*condition.felt_temperature_morning),
            units.format_temperature(condition.felt_temperature_day),
            units.format_temperature(*condition.felt_temperature_evening),
            units.format_temperature(*condition.felt_temperature_night))});
  } else {
    this->description_data.push_back(
        RowDescription{GetLangText("Temperatures"), "", true});

    this->description_data.push_back(
        RowDescription{GetLangText("Mean"),
                       units.format_temperature(condition.temperature_day)});
    this->description_data.push_back(RowDescription{
        GetLangText("Felt"),
        units.format_temperature(condition.felt_temperature_day)});
  }
  this->description_data.push_back(
      RowDescription{GetLangText("Min/Max"),
                     units.format_temperature(condition.temperature_min) + "/" +
                         units.format_temperature(condition.temperature_max)});

  this->description_data.push_back(empty_row);

  if (static_cast<int>(condition.wind_speed) != 0 or
      static_cast<int>(condition.wind_gust) != 0) {

    this->description_data.push_back(
        RowDescription{GetLangText("Wind"), "", true});

    const auto wind_speed_text = units.format_speed(condition.wind_speed);
    if (static_cast<int>(condition.wind_speed) != 0) {
      this->description_data.push_back(
          RowDescription{GetLangText("Speed"), wind_speed_text});
    }

    if (static_cast<int>(condition.wind_gust) != 0) {
      const auto wind_gust_text = units.format_speed(condition.wind_gust);
      if (condition.wind_gust > condition.wind_speed and
          wind_speed_text != wind_gust_text) {
        this->description_data.push_back(
            RowDescription{GetLangText("Gust"), wind_gust_text});
      }
    }

    if (static_cast<int>(condition.wind_speed) != 0 or
        static_cast<int>(condition.wind_gust) != 0) {

      const auto arrow_angle = (180 - condition.wind_degree);
      // The parameter degree is an angle measure in degrees, interpreted
      // as the direction where the wind is blowing FROM (0 means North,
      // 90 East), but the icon arrow must show where the wind is blowing
      // TO. Whats more OpenCV rotation is counter-clockwise for positive
      // angle values.

      this->description_data.push_back(
          RowDescription{GetLangText("Direction"),
                         std::make_pair("direction", arrow_angle), false});
    }

    this->description_data.push_back(empty_row);
  }

  if (not std::isnan(condition.rain) or not std::isnan(condition.snow)) {

    this->description_data.push_back(
        RowDescription{GetLangText("Precipitation"), "", true});

    if (not std::isnan(condition.rain)) {
      this->description_data.push_back(
          RowDescription{GetLangText("Rain"),
                         units.format_precipitation(condition.rain, true)});
    }

    if (not std::isnan(condition.snow)) {
      this->description_data.push_back(
          RowDescription{GetLangText("Snow"),
                         units.format_precipitation(condition.snow, true)});
    }
    if (not std::isnan(condition.probability_of_precipitation)) {
      this->description_data.push_back(
          RowDescription{GetLangText("Probability"),
                         std::to_string(static_cast<int>(
                             condition.probability_of_precipitation * 100)) +
                             "%"});
    }
    this->description_data.push_back(empty_row);
  }

  this->description_data.push_back(
      RowDescription{GetLangText("Other metrics"), "", true});

  this->description_data.push_back(
      RowDescription{std::string{GetLangText("Pressure")},
                     units.format_pressure(condition.pressure)});

  this->description_data.push_back(
      RowDescription{std::string{GetLangText("Humidity")},
                     std::to_string(condition.humidity) + "%"});

  if (not std::isnan(condition.dew_point)) {
    this->description_data.push_back(
        RowDescription{GetLangText("Dew point"),
                       units.format_temperature(condition.uv_index)});
  }

  if (not std::isnan(condition.uv_index)) {
    this->description_data.push_back(
        RowDescription{GetLangText("UV index"),
                       std::to_string(static_cast<int>(condition.uv_index))});
  }

  if (not std::isnan(condition.clouds)) {
    this->description_data.push_back(RowDescription{
        GetLangText("Cloudiness"), std::to_string(condition.clouds) + "%"});
  }
}

bool DailyForecastViewer::handle_key_release(int key) {
  if (key == IV_KEY_PREV) {
    this->display_previous_forecast_maybe();
  } else if (key == IV_KEY_NEXT) {
    this->display_next_forecast_maybe();
  }
  return true;
}

int DailyForecastViewer::handle_pointer_event(int event_type, int pointer_pos_x,
                                              int pointer_pos_y) {
  if (this->handle_possible_swipe(event_type, pointer_pos_x, pointer_pos_y)) {
    return 1;
  }
  if (event_type == EVT_SCROLL) {
    auto *const scroll_area = reinterpret_cast<irect *>(pointer_pos_x);
    if (scroll_area == &this->scrollable_view_rectangle) {
      const int delta_x = pointer_pos_y >> 16;
      const int delta_y = (pointer_pos_y << 16) >> 16;
      BOOST_LOG_TRIVIAL(debug)
          << "Daily forecast viewer received a scrolling event "
          << "delta: " << delta_x << " " << delta_y
          << " current offset: " << this->scrollable_view_offset
          << " min. offset: " << this->min_scrollable_view_offset;

      this->scrollable_view_offset =
          std::min(0, std::max(this->scrollable_view_offset + delta_y,
                               this->min_scrollable_view_offset));

      this->paint_and_update_screen();

      return 1;
    }
  }
  if (this->close_button and
      this->close_button->is_in_bouding_box(pointer_pos_x, pointer_pos_y) and
      this->close_button->is_enabled()) {
    return this->close_button->handle_pointer_event(event_type, pointer_pos_x,
                                                    pointer_pos_y);
  }
  return 0;
}

void DailyForecastViewer::display_previous_forecast_maybe() {
  if (this->forecast_index != 0) {
    --this->forecast_index;

    const auto &condition =
        this->model->daily_forecast.at(this->forecast_index);
    this->generate_description_data(condition);

    this->identify_scrollable_area();

    this->paint_and_update_screen();
  } else {
    this->hide();
  }
}

void DailyForecastViewer::display_next_forecast_maybe() {
  if (this->forecast_index + 1 < this->model->daily_forecast.size()) {
    ++this->forecast_index;

    const auto &condition =
        this->model->daily_forecast.at(this->forecast_index);
    this->generate_description_data(condition);

    this->identify_scrollable_area();

    this->paint_and_update_screen();
  } else {
    this->hide();
  }
}

bool DailyForecastViewer::handle_possible_swipe(int event_type,
                                                int pointer_pos_x,
                                                int pointer_pos_y) {
  const auto swipe = this->swipe_detector.guess_event_swipe_type(
      event_type, pointer_pos_x, pointer_pos_y);
  if (swipe != SwipeType::no_swipe) {
    if (swipe == SwipeType::left_swipe) {
      this->display_next_forecast_maybe();
    } else if (swipe == SwipeType::right_swipe) {
      this->display_previous_forecast_maybe();
    }
    return true;
  }
  return false;
}
} // namespace taranis
