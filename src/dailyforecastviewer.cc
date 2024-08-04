#include "dailyforecastviewer.h"

#include "units.h"
#include "util.h"

namespace taranis {

DailyForecastViewer::DailyForecastViewer(std::shared_ptr<Model> model,
                                         std::shared_ptr<Fonts> fonts)
    : ModalWidget{}, model{model}, fonts{fonts},
      content_width{this->bounding_box.w -
                    2 * DailyForecastViewer::horizontal_padding},
      title_height{2 * this->fonts->get_small_font()->height},
      forecast_title_start_y{this->title_height +
                             DailyForecastViewer::vertical_padding} {}

void DailyForecastViewer::open() {
  if (this->forecast_count() == 0 or
      this->forecast_index >= this->forecast_count()) {
    this->hide();
    return;
  }
  this->visible = true;

  this->update_title_text();

  const auto &condition = this->model->daily_forecast.at(this->forecast_index);
  this->update_forecast_title_text(condition);
  this->generate_description_data(condition);

  this->update_layout();

  AddScrolledArea(&this->scrollable_view_rectangle, true);

  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_SHOW, 0, 0);
}

void DailyForecastViewer::hide() {
  this->visible = false;
  RemoveScrolledArea(&this->scrollable_view_rectangle);

  this->forecast_index = 0;

  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_SHOW, 0, 0);
}

void DailyForecastViewer::set_forecast_index(size_t index) {
  this->forecast_index = index;
}

void DailyForecastViewer::do_paint() {
  // title
  const auto default_font = this->fonts->get_small_font();
  SetFont(default_font.get(), BLACK);

  DrawTextRect(DailyForecastViewer::horizontal_padding,
               DailyForecastViewer::vertical_padding, this->content_width,
               this->title_height, this->title_text.c_str(), ALIGN_CENTER);

  DrawHorizontalSeparator(0, this->title_height, ScreenWidth(),
                          HORIZONTAL_SEPARATOR_SOLID);

  // forecast title
  const auto bold_font = this->fonts->get_small_bold_font();
  SetFont(bold_font.get(), BLACK);

  const auto forecast_title_height = TextRectHeight(
      this->content_width, this->forecast_title_text.c_str(), ALIGN_LEFT);
  DrawTextRect(DailyForecastViewer::horizontal_padding,
               this->forecast_title_start_y, this->content_width,
               forecast_title_height, this->forecast_title_text.c_str(),
               ALIGN_LEFT);

  // Forecast description
  SetFont(default_font.get(), BLACK);

  SetClipRect(&this->scrollable_view_rectangle);

  int current_row_start_y =
      this->scrollable_view_rectangle.y + this->scrollable_view_offset;
  const int second_column_of_two_x =
      this->scrollable_view_rectangle.x + this->scrollable_view_rectangle.w / 2;
  const int second_column_of_three_x =
      this->scrollable_view_rectangle.x + this->scrollable_view_rectangle.w / 3;
  const int third_column_of_three_x = this->scrollable_view_rectangle.x +
                                      2 * this->scrollable_view_rectangle.w / 3;

  std::string label_text, first_text, second_text, third_text;

  for (auto const &row_values : this->description_data) {
    if (row_values.type() == typeid(std::string)) {

      label_text = boost::get<std::string>(row_values);

      DrawTextRect(DailyForecastViewer::horizontal_padding, current_row_start_y,
                   StringWidth(label_text.c_str()), default_font.get()->height,
                   label_text.c_str(), ALIGN_LEFT);

    } else if (row_values.type() ==
               typeid(std::pair<std::string, std::string>)) {

      std::tie(label_text, first_text) =
          boost::get<std::pair<std::string, std::string>>(row_values);

      DrawTextRect(DailyForecastViewer::horizontal_padding, current_row_start_y,
                   StringWidth(label_text.c_str()), default_font.get()->height,
                   label_text.c_str(), ALIGN_LEFT);

      DrawTextRect(
          DailyForecastViewer::horizontal_padding + second_column_of_two_x,
          current_row_start_y, StringWidth(first_text.c_str()),
          default_font.get()->height, first_text.c_str(), ALIGN_CENTER);

    } else if (row_values.type() ==
               typeid(std::tuple<std::string, std::string, std::string>)) {

      std::tie(label_text, first_text, second_text) =
          boost::get<std::tuple<std::string, std::string, std::string>>(
              row_values);

      DrawTextRect(DailyForecastViewer::horizontal_padding, current_row_start_y,
                   StringWidth(label_text.c_str()), default_font.get()->height,
                   label_text.c_str(), ALIGN_LEFT);

      DrawTextRect(
          DailyForecastViewer::horizontal_padding + second_column_of_three_x,
          current_row_start_y, StringWidth(first_text.c_str()),
          default_font.get()->height, first_text.c_str(), ALIGN_CENTER);

      DrawTextRect(
          DailyForecastViewer::horizontal_padding + third_column_of_three_x,
          current_row_start_y, StringWidth(second_text.c_str()),
          default_font.get()->height, second_text.c_str(), ALIGN_CENTER);
    } else if (row_values.type() == typeid(std::pair<std::string, ibitmap *>)) {
      // not implemented
    }

    current_row_start_y += default_font->height;
  }

  this->forecast_description_height =
      current_row_start_y -
      (this->scrollable_view_rectangle.y + this->scrollable_view_offset);

  SetClip(0, 0, ScreenWidth(), ScreenHeight());
}

void DailyForecastViewer::update_layout() {
  const auto bold_font = this->fonts->get_small_bold_font();
  const auto default_font = this->fonts->get_small_font();

  SetFont(bold_font.get(), BLACK);

  const auto forecast_title_height = TextRectHeight(
      this->content_width, this->forecast_title_text.c_str(), ALIGN_LEFT);

  SetFont(default_font.get(), BLACK);

  const auto scrollable_view_start_y = this->forecast_title_start_y +
                                       forecast_title_height +
                                       DailyForecastViewer::vertical_padding;
  this->scrollable_view_rectangle =
      iRect(DailyForecastViewer::horizontal_padding, scrollable_view_start_y,
            ScreenWidth() - 2 * DailyForecastViewer::horizontal_padding,
            ScreenHeight() - scrollable_view_start_y -
                DailyForecastViewer::vertical_padding,
            0);

  this->scrollable_view_offset = 0;

  const int description_height =
      this->description_data.size() * default_font->height;

  this->min_scrollable_view_offset =
      -std::max(0, description_height - this->scrollable_view_rectangle.h);
}

void DailyForecastViewer::update_title_text() {
  this->title_text = std::string{GetLangText("DAILY FORECAST")};
  if (this->forecast_count() > 1) {
    this->title_text += (" " + std::to_string(this->forecast_index + 1) + "/" +
                         std::to_string(this->forecast_count()));
  }
}

void DailyForecastViewer::update_forecast_title_text(
    const DailyCondition &condition) {
  this->forecast_title_text = format_date(condition.date, true);
}

void DailyForecastViewer::generate_description_data(
    const DailyCondition &condition) {
  this->description_data.clear();

  const Units units{this->model};

  this->description_data.push_back(std::tuple{std::string{GetLangText("Sun")},
                                              format_time(condition.sunrise),
                                              format_time(condition.sunset)});

  this->description_data.push_back(std::tuple{std::string{GetLangText("Moon")},
                                              format_time(condition.moonrise),
                                              format_time(condition.moonset)});
  // TODO Moon phase

  this->description_data.push_back("");

  std::string description_text = condition.weather_description;
  if (not description_text.empty()) {
    description_text[0] = std::toupper(description_text[0]);
  }
  this->description_data.push_back(
      std::pair{GetLangText("Weather"), description_text});

  this->description_data.push_back("");

  this->description_data.push_back(std::tuple{
      std::string{}, GetLangText("Temperature"), GetLangText("Felt")});
  this->description_data.push_back(
      std::tuple{GetLangText("Morning"),
                 units.format_temperature(condition.temperature_morning),
                 units.format_temperature(condition.felt_temperature_morning)});
  this->description_data.push_back(std::tuple{
      GetLangText("Day"), units.format_temperature(condition.temperature_day),
      units.format_temperature(condition.felt_temperature_day)});
  this->description_data.push_back(
      std::tuple{GetLangText("Evening"),
                 units.format_temperature(condition.temperature_evening),
                 units.format_temperature(condition.felt_temperature_evening)});
  this->description_data.push_back(
      std::tuple{GetLangText("Night"),
                 units.format_temperature(condition.temperature_night),
                 units.format_temperature(condition.felt_temperature_night)});
  this->description_data.push_back(
      std::pair{GetLangText("Min/Max"),
                units.format_temperature(condition.temperature_min) + "/" +
                    units.format_temperature(condition.temperature_max)});

  this->description_data.push_back("");

  this->description_data.push_back(
      std::pair{std::string{GetLangText("Pressure")},
                units.format_pressure(condition.pressure)});

  this->description_data.push_back(
      std::pair{std::string{GetLangText("Humidity")},
                std::to_string(condition.humidity) + "%"});

  if (not std::isnan(condition.dew_point)) {
    this->description_data.push_back(
        std::pair{GetLangText("Dew point"),
                  units.format_temperature(condition.uv_index)});
  }

  if (not std::isnan(condition.uv_index)) {
    this->description_data.push_back(
        std::pair{GetLangText("UV index"),
                  std::to_string(static_cast<int>(condition.uv_index))});
  }

  if (not std::isnan(condition.clouds)) {
    this->description_data.push_back(std::pair{
        GetLangText("Cloudiness"), std::to_string(condition.clouds) + "%"});
  }

  this->description_data.push_back("");

  if (static_cast<int>(condition.wind_speed) != 0 or
      static_cast<int>(condition.wind_gust) != 0) {
    const auto wind_speed_text = units.format_speed(condition.wind_speed);
    if (static_cast<int>(condition.wind_speed) != 0) {
      this->description_data.push_back(
          std::pair{GetLangText("Wind speed"), wind_speed_text});
    }

    if (static_cast<int>(condition.wind_gust) != 0) {
      const auto wind_gust_text = units.format_speed(condition.wind_gust);
      if (condition.wind_gust > condition.wind_speed and
          wind_speed_text != wind_gust_text) {
        this->description_data.push_back(
            std::pair{GetLangText("Wind gust"), wind_gust_text});
      }
    }
    // TODO wind degree

    this->description_data.push_back("");
  }

  if (not std::isnan(condition.rain) or not std::isnan(condition.snow)) {
    if (not std::isnan(condition.rain)) {
      this->description_data.push_back(
          std::pair{GetLangText("Rain"),
                    units.format_precipitation(condition.rain, true)});
    }

    if (not std::isnan(condition.snow)) {
      this->description_data.push_back(
          std::pair{GetLangText("Snow"),
                    units.format_precipitation(condition.snow, true)});
    }
    if (not std::isnan(condition.probability_of_precipitation)) {
      this->description_data.push_back(
          std::pair{GetLangText("Probability"),
                    std::to_string(static_cast<int>(
                        condition.probability_of_precipitation * 100)) +
                        "%"});
    }
  }
}

void DailyForecastViewer::display_previous_forecast_maybe() {
  if (this->forecast_index != 0) {
    --this->forecast_index;

    const auto &condition =
        this->model->daily_forecast.at(this->forecast_index);
    this->update_title_text();
    this->update_forecast_title_text(condition);
    this->generate_description_data(condition);

    this->update_layout();

    this->paint_and_update_screen();
  } else {
    this->hide();
  }
}

void DailyForecastViewer::display_next_forecast_maybe() {
  if (this->forecast_index + 1 < this->forecast_count()) {
    ++this->forecast_index;

    const auto &condition =
        this->model->daily_forecast.at(this->forecast_index);
    this->update_title_text();
    this->update_forecast_title_text(condition);
    this->generate_description_data(condition);

    this->update_layout();

    this->paint_and_update_screen();
  } else {
    this->hide();
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
  return 0;
}

} // namespace taranis
