#pragma once

#include <inkview.h>
#include <iomanip>
#include <memory>
#include <sstream>

#include "fonts.h"
#include "model.h"
#include "units.h"
#include "widget.h"

#define T(x) GetLangText(x)

namespace taranis {

void handle_current_condition_dialog_button_clicked(int button_index);

class CurrentConditionBox : public Widget {
public:
  CurrentConditionBox(int pos_x, int pos_y, std::shared_ptr<Model> model,
                      std::shared_ptr<Fonts> fonts)
      : Widget{pos_x, pos_y}, model{model}, fonts{fonts} {
    this->set_width(4 * ScreenWidth() / 5);
    this->set_height(this->fonts->get_big_font()->height);
  }

  void show() override {
    this->fill_bounding_box();

    const auto condition = this->model->current_condition;
    if (not condition) {
      return;
    }

    const Units units{this->model};

    const std::string temperature_text =
        units.format_temperature(static_cast<int>(condition->temperature));

    const auto temperature_pos_x =
        this->bounding_box.x + this->horizontal_padding;
    const auto temperature_pos_y = this->bounding_box.y;
    SetFont(this->fonts->get_big_font().get(), BLACK);
    DrawString(temperature_pos_x, temperature_pos_y, temperature_text.c_str());

    const auto description_pos_x = temperature_pos_x +
                                   StringWidth(temperature_text.c_str()) +
                                   this->horizontal_padding;
    const auto description_pos_y = temperature_pos_y +
                                   this->fonts->get_big_font()->height -
                                   3 * this->fonts->get_small_font()->height;
    const auto description_text =
        CurrentConditionBox::get_description_text(*condition);

    const auto felt_temperature_pos_x = description_pos_x;
    const auto felt_temperature_pos_y =
        description_pos_y + this->fonts->get_small_font()->height;
    const auto felt_temperature_text =
        this->get_felt_temperature_text(*condition);

    SetFont(this->fonts->get_small_font().get(), BLACK);
    DrawString(description_pos_x, description_pos_y, description_text.c_str());
    DrawString(felt_temperature_pos_x, felt_temperature_pos_y,
               felt_temperature_text.c_str());
  }

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override {
    if (event_type == EVT_POINTERUP) {
      // this->open_current_condition_dialog();
      // return 1;

      // Currently disabled since rendering is ugly.
      // See https://github.com/orontee/taranis/issues/16
    }
    return 0;
  }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<Fonts> fonts;

  const int horizontal_padding{50};

  static std::string get_description_text(const Condition &condition) {
    std::string description_text = condition.weather_description;
    if (not description_text.empty()) {
      description_text[0] = std::toupper(description_text[0]);
    }
    return description_text;
  }

  std::string get_felt_temperature_text(const Condition &condition) const {
    const Units units{this->model};
    std::stringstream text;
    text << T("Felt") << " "
         << units.format_temperature(
                static_cast<int>(condition.felt_temperature));

    return text.str();
  }

  void open_current_condition_dialog() {
    if (not this->model->current_condition) {
      return;
    }
    const auto condition = *(this->model->current_condition);
    const Units units{this->model};

    const char *const time_format = "%H:%M";
    std::string sunrise_text{"?????"};
    std::strftime(const_cast<char *>(sunrise_text.c_str()), 6, time_format,
                  std::localtime(&condition.sunrise));
    std::string sunset_text{"?????"};
    std::strftime(const_cast<char *>(sunset_text.c_str()), 6, time_format,
                  std::localtime(&condition.sunset));

    std::stringstream content;
    content << T("Sunrise") << std::right << std::setw(10) << sunrise_text
            << std::endl
            << T("Sunset") << std::right << std::setw(10) << sunset_text
            << std::endl
            << T("Pressure") << std::right << std::setw(10)
            << units.format_pressure(condition.pressure) << std::endl
            << T("Humidity") << std::right << std::setw(10)
            << std::to_string(condition.humidity) + "%" << std::endl
            << T("UV index") << std::right << std::setw(10) << std::fixed
            << std::setprecision(1) << condition.uv_index << std::endl
            << T("Visibility") << std::right << std::setw(10)
            << units.format_distance(condition.visibility) << std::endl
            << T("Wind") << std::right << std::setw(10)
            << units.format_speed(condition.wind_speed) << std::endl
            << T("Gust") << std::right << std::setw(10)
            << units.format_speed(condition.wind_gust) << std::endl;

    Dialog(ICON_INFORMATION, T("Current Weather Conditions"),
           content.str().c_str(), T("Ok"), nullptr,
           &handle_current_condition_dialog_button_clicked);
  }
};

} // namespace taranis
