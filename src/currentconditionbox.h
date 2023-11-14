#pragma once

#include <inkview.h>
#include <iomanip>
#include <memory>
#include <sstream>

#include "fonts.h"
#include "model.h"
#include "units.h"
#include "widget.h"

namespace taranis {

class CurrentConditionBox : public Widget {
public:
  static constexpr int bottom_padding{25};

  CurrentConditionBox(int pos_x, int pos_y, std::shared_ptr<Model> model,
                      std::shared_ptr<Fonts> fonts)
      : Widget{pos_x, pos_y}, model{model}, fonts{fonts} {
    this->set_width(4 * ScreenWidth() / 5);
    this->set_height(this->fonts->get_big_font()->height +
                     this->bottom_padding);
  }

  void do_paint() override {
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

private:
  static constexpr int horizontal_padding{50};

  std::shared_ptr<Model> model;
  std::shared_ptr<Fonts> fonts;

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
    text << GetLangText("Felt") << " "
         << units.format_temperature(
                static_cast<int>(condition.felt_temperature));

    return text.str();
  }
};

} // namespace taranis
