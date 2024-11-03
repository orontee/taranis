#include "dailyforecastviewer.h"

#include "inkview.h"
#include "units.h"
#include "util.h"

namespace taranis {

DailyForecastViewer::DailyForecastViewer(std::shared_ptr<Model> model,
                                         std::shared_ptr<Icons> icons,
                                         std::shared_ptr<Fonts> fonts)
    : ModalWidget{DailyForecastViewer::horizontal_padding, ScreenHeight() / 3,
                  ScreenWidth() - 2 * DailyForecastViewer::horizontal_padding,
                  ScreenHeight() - ScreenHeight() / 3 -
                      2 * DailyForecastViewer::vertical_padding},
      model{model}, icons{icons}, fonts{fonts},
      direction_icon{BitmapStretchProportionally(
          icons->get("direction"), fonts->get_small_font()->size,
          fonts->get_small_font()->size)} {}

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
  DrawFrameRectCertified(this->bounding_box, THICKNESS_DEF_FOCUSED_FRAME);

  const auto default_font = this->fonts->get_small_font();
  const auto bold_font = this->fonts->get_small_bold_font();

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

  DrawHorizontalSeparator(
      this->get_pos_x() + DailyForecastViewer::horizontal_padding,
      this->get_pos_y() + title_height,
      this->get_width() - 2 * DailyForecastViewer::horizontal_padding,
      HORIZONTAL_SEPARATOR_SOLID);

  if (!this->close_button) {
    const auto close_button_icon_size =
        bold_font->height + DailyForecastViewer::vertical_padding;

    this->close_button =
        std::make_shared<CloseButton>(close_button_icon_size, this->icons);
    this->close_button->set_click_handler(
        std::bind(&DailyForecastViewer::hide, this));
    this->close_button->set_pos_x(this->get_width() - close_button_icon_size);
    this->close_button->set_pos_y(this->get_pos_y() +
                                  DailyForecastViewer::vertical_padding / 4);
  }
  this->close_button->do_paint();

  // forecast
  SetFont(default_font.get(), BLACK);

  SetClipRect(&this->scrollable_view_rectangle);

  int current_row_start_y = this->scrollable_view_rectangle.y +
                            DailyForecastViewer::vertical_padding +
                            this->scrollable_view_offset;
  const int second_column_of_two_x =
      this->scrollable_view_rectangle.x + this->scrollable_view_rectangle.w / 2;
  const int second_column_of_three_x =
      this->scrollable_view_rectangle.x + this->scrollable_view_rectangle.w / 3;
  const int third_column_of_three_x = this->scrollable_view_rectangle.x +
                                      2 * this->scrollable_view_rectangle.w / 3;

  std::string label_text, first_text, second_text, third_text;
  int degree;

  for (auto const &row_values : this->description_data) {
    if (row_values.type() == typeid(std::string)) {

      SetFont(bold_font.get(), BLACK);

      label_text = boost::get<std::string>(row_values);

      DrawTextRect(this->get_pos_x() + DailyForecastViewer::horizontal_padding,
                   current_row_start_y, StringWidth(label_text.c_str()),
                   bold_font.get()->height, label_text.c_str(), ALIGN_LEFT);

      current_row_start_y += bold_font->height;
    } else if (row_values.type() ==
               typeid(std::pair<std::string, std::string>)) {

      SetFont(default_font.get(), BLACK);

      std::tie(label_text, first_text) =
          boost::get<std::pair<std::string, std::string>>(row_values);

      DrawTextRect(this->get_pos_x() + DailyForecastViewer::horizontal_padding,
                   current_row_start_y, StringWidth(label_text.c_str()),
                   default_font.get()->height, label_text.c_str(), ALIGN_LEFT);

      DrawTextRect(this->get_pos_x() + DailyForecastViewer::horizontal_padding +
                       second_column_of_two_x,
                   current_row_start_y,
                   this->get_width() -
                       2 * DailyForecastViewer::horizontal_padding -
                       second_column_of_two_x,
                   default_font.get()->height, first_text.c_str(), ALIGN_RIGHT);

      current_row_start_y += default_font->height;
    } else if (row_values.type() ==
               typeid(std::tuple<std::string, std::string, std::string>)) {

      std::tie(label_text, first_text, second_text) =
          boost::get<std::tuple<std::string, std::string, std::string>>(
              row_values);

      SetFont(default_font.get(), BLACK);

      DrawTextRect(this->get_pos_x() + DailyForecastViewer::horizontal_padding,
                   current_row_start_y, StringWidth(label_text.c_str()),
                   default_font.get()->height, label_text.c_str(), ALIGN_LEFT);

      DrawTextRect(this->get_pos_x() + DailyForecastViewer::horizontal_padding +
                       second_column_of_three_x,
                   current_row_start_y, StringWidth(first_text.c_str()),
                   default_font.get()->height, first_text.c_str(),
                   ALIGN_CENTER);

      DrawTextRect(this->get_pos_x() + DailyForecastViewer::horizontal_padding +
                       third_column_of_three_x,
                   current_row_start_y, StringWidth(second_text.c_str()),
                   default_font.get()->height, second_text.c_str(),
                   ALIGN_CENTER);

      current_row_start_y += default_font->height;
    } else if (row_values.type() == typeid(std::pair<std::string, int>)) {
      std::tie(label_text, degree) =
          boost::get<std::pair<std::string, int>>(row_values);

      SetFont(default_font.get(), BLACK);

      DrawTextRect(this->get_pos_x() + DailyForecastViewer::horizontal_padding,
                   current_row_start_y, StringWidth(label_text.c_str()),
                   default_font.get()->height, label_text.c_str(), ALIGN_LEFT);

      this->rotated_icon_data = this->icons->rotate_icon(
          const_cast<ibitmap *>(direction_icon), degree);
      const auto icon =
          reinterpret_cast<ibitmap *>(this->rotated_icon_data.data());
      if (icon) {
        DrawBitmap(this->get_pos_x() + this->get_width() -
                       DailyForecastViewer::horizontal_padding - icon->width,
                   current_row_start_y, icon);
      }
      current_row_start_y += default_font->height;
    }
  }
  SetClip(0, 0, ScreenWidth(), ScreenHeight());
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
      2 * DailyForecastViewer::vertical_padding;

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
  this->description_data.push_back(std::pair{description_text, std::string{}});

  // TODO Weather icon

  // TODO Secondary description

  this->description_data.push_back("");

  this->description_data.push_back(std::tuple{std::string{GetLangText("Sun")},
                                              format_time(condition.sunrise),
                                              format_time(condition.sunset)});

  this->description_data.push_back(std::tuple{std::string{GetLangText("Moon")},
                                              format_time(condition.moonrise),
                                              format_time(condition.moonset)});
  // TODO Moon phase

  this->description_data.push_back("");

  this->description_data.push_back(GetLangText("Temperatures"));
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
      std::tuple{GetLangText("Min/Max"),
                 units.format_temperature(condition.temperature_min),
                 units.format_temperature(condition.temperature_max)});

  this->description_data.push_back("");

  if (static_cast<int>(condition.wind_speed) != 0 or
      static_cast<int>(condition.wind_gust) != 0) {

    this->description_data.push_back(std::string{GetLangText("Wind")});

    const auto wind_speed_text = units.format_speed(condition.wind_speed);
    if (static_cast<int>(condition.wind_speed) != 0) {
      this->description_data.push_back(
          std::pair{GetLangText("Speed"), wind_speed_text});
    }

    if (static_cast<int>(condition.wind_gust) != 0) {
      const auto wind_gust_text = units.format_speed(condition.wind_gust);
      if (condition.wind_gust > condition.wind_speed and
          wind_speed_text != wind_gust_text) {
        this->description_data.push_back(
            std::pair{GetLangText("Gust"), wind_gust_text});
      }
    }

    if (static_cast<int>(condition.wind_speed) != 0 or
        static_cast<int>(condition.wind_gust) != 0) {
      this->description_data.push_back(
          std::pair{GetLangText("Direction"), condition.wind_degree});
    }

    this->description_data.push_back("");
  }

  if (not std::isnan(condition.rain) or not std::isnan(condition.snow)) {

    this->description_data.push_back(std::string{GetLangText("Precipitation")});

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
    this->description_data.push_back("");
  }

  this->description_data.push_back(std::string{GetLangText("Other metrics")});

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
}

bool DailyForecastViewer::handle_key_release(int key) {
  if (key == IV_KEY_PREV or key == IV_KEY_NEXT) {
    this->hide();
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
  if (this->close_button and
      this->close_button->is_in_bouding_box(pointer_pos_x, pointer_pos_y) and
      this->close_button->is_enabled()) {
    return this->close_button->handle_pointer_event(event_type, pointer_pos_x,
                                                    pointer_pos_y);
  }
  return 0;
}

} // namespace taranis
