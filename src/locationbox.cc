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
  const std::string location_text = format_location(this->model->location);

  const auto label_pos_x = this->bounding_box.x + this->left_padding;
  const auto label_pos_y = this->bounding_box.y + this->top_padding;

  const auto content_width =
      this->bounding_box.w - this->left_padding - this->right_padding;
  const auto content_height =
      this->bounding_box.h - this->top_padding - this->bottom_padding;

  SetFont(this->font.get(), BLACK);

  DrawTextRect(label_pos_x, label_pos_y, content_width, content_height,
               location_text.c_str(), ALIGN_LEFT | VALIGN_TOP | DOTS);
}
} // namespace taranis
