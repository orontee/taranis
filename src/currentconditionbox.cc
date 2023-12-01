#include "currentconditionbox.h"

#include <inkview.h>
#include <iomanip>
#include <sstream>

#include "units.h"

namespace taranis {
CurrentConditionBox::CurrentConditionBox(int pos_x, int pos_y, int width,
                                         std::shared_ptr<Model> model,
                                         std::shared_ptr<Fonts> fonts)
    : Widget{pos_x, pos_y}, model{model}, fonts{fonts} {
  this->set_width(width);
  this->set_height(this->fonts->get_big_font()->height + this->bottom_padding +
                   this->top_padding);
}

void CurrentConditionBox::do_paint() {
  const auto condition = this->model->current_condition;
  if (not condition) {
    return;
  }

  const Units units{this->model};

  const auto temperature_pos_x = this->bounding_box.x + this->left_padding;
  const auto temperature_pos_y = this->bounding_box.y + this->top_padding;

  SetFont(this->fonts->get_big_font().get(), BLACK);

  const auto temperature_text =
      units.format_temperature(static_cast<int>(condition->temperature));

  const auto temperature_text_width = StringWidth(temperature_text.c_str());

  DrawString(temperature_pos_x, temperature_pos_y, temperature_text.c_str());

  const auto remaining_width = this->bounding_box.w - this->left_padding -
                               this->right_padding - temperature_text_width -
                               this->inner_horizontal_spacing;

  if (remaining_width <= 0) {
    BOOST_LOG_TRIVIAL(warning) << "No space left for description text!";
    return;
  }

  const auto description_pos_x = temperature_pos_x + temperature_text_width +
                                 this->inner_horizontal_spacing;
  const auto description_pos_y = temperature_pos_y +
                                 this->fonts->get_big_font()->height -
                                 3 * this->fonts->get_small_font()->height;
  const auto description_text =
      CurrentConditionBox::get_description_text(*condition);

  const auto felt_temperature_pos_x = description_pos_x;
  const auto felt_temperature_pos_y = description_pos_y +
                                      this->fonts->get_small_font()->height +
                                      this->inner_vertical_spacing;
  const auto felt_temperature_text =
      this->get_felt_temperature_text(*condition);

  SetFont(this->fonts->get_small_font().get(), BLACK);

  DrawTextRect(description_pos_x, description_pos_y, remaining_width,
               this->fonts->get_small_font()->height, description_text.c_str(),
               ALIGN_LEFT | DOTS);
  DrawTextRect(felt_temperature_pos_x, felt_temperature_pos_y, remaining_width,
               this->fonts->get_small_font()->height,
               felt_temperature_text.c_str(), ALIGN_LEFT | DOTS);
}

std::string
CurrentConditionBox::get_description_text(const Condition &condition) {
  std::string description_text = condition.weather_description;
  if (not description_text.empty()) {
    description_text[0] = std::toupper(description_text[0]);
  }
  return description_text;
}

std::string CurrentConditionBox::get_felt_temperature_text(
    const Condition &condition) const {
  const Units units{this->model};
  std::stringstream text;
  text << GetLangText("Felt") << " "
       << units.format_temperature(
              static_cast<int>(condition.felt_temperature));

  return text.str();
}
} // namespace taranis
