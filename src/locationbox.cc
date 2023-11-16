#include "locationbox.h"

#include <array>
#include <boost/log/trivial.hpp>
#include <cmath>
#include <cstring>
#include <inkview.h>
#include <sstream>
#include <string>

#include "events.h"
#include "util.h"

namespace taranis {

LocationBox::LocationBox(int pos_x, int pos_y, std::shared_ptr<Model> model,
                         std::shared_ptr<Fonts> fonts)
    : Widget{pos_x, pos_y}, model{model}, font{fonts->get_bold_font()} {
  this->set_width(4 * ScreenWidth() / 5);
  this->set_height(this->font->height + this->top_padding +
                   this->bottom_padding);
}

int LocationBox::handle_pointer_event(int event_type, int pointer_pos_x,
                                      int pointer_pos_y) {
  if (event_type == EVT_POINTERUP) {
    this->edit_location();
    return 1;
  }
  return 0;
}

std::array<char, 256> input_text;

void LocationBox::keyboard_handler(char *text) {
  const auto event_handler = GetEventHandler();
  SendEventEx(event_handler, EVT_CUSTOM, CustomEvent::search_location, 0,
              &input_text);
}

void LocationBox::edit_location() {
  BOOST_LOG_TRIVIAL(debug) << "Will open keyboard to edit location";

  const auto title = GetLangText("Enter location");

  std::memset(input_text.data(), '\0', input_text.size());
  const auto formatted_location_name = format_location(this->model->location);
  std::memcpy(input_text.data(), formatted_location_name.data(),
              formatted_location_name.size());
  OpenKeyboard(title, input_text.data(), input_text.size() - 1, KBD_NORMAL,
               LocationBox::keyboard_handler);
}

void LocationBox::do_paint() {
  std::string location_text = format_location(this->model->location);
  location_text = elide_maybe(location_text);

  const auto label_pos_x = this->bounding_box.x + this->left_padding;
  const auto label_pos_y = this->bounding_box.y + this->top_padding;

  SetFont(this->font.get(), BLACK);

  DrawString(label_pos_x, label_pos_y, location_text.c_str());
}

std::string LocationBox::elide_maybe(const std::string &text) const {
  SetFont(this->font.get(), BLACK);
  const auto text_width = StringWidth(text.c_str());
  if (text_width <= this->bounding_box.w or text.size() <= 2) {
    // the second case should not happen unless the default font is
    // huge...
    return text;
  }
  size_t keep = text.size() - 2;
  std::string elided_text = text.substr(0, keep) + "…";
  while (StringWidth(elided_text.c_str()) > this->bounding_box.w) {
    --keep;
    elided_text = text.substr(0, keep) + "…";
  }
  return elided_text;
}
} // namespace taranis
