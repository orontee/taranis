#pragma once

#include <array>
#include <cmath>
#include <cstring>
#include <inkview.h>
#include <memory>
#include <sstream>
#include <string>

#include "fonts.h"
#include "model.h"
#include "util.h"
#include "widget.h"

#define T(x) GetLangText(x)

using namespace std::string_literals;

namespace taranis {

void keyboard_handler(char *text);

class LocationBox : public Widget {
public:
  LocationBox(int pos_x, int pos_y, std::shared_ptr<Model> model,
              std::shared_ptr<Fonts> fonts)
      : Widget{pos_x, pos_y}, model{model}, font{fonts->get_bold_font()} {
    this->set_width(4 * ScreenWidth() / 5);
    this->set_height(this->font->height + this->top_padding +
                     this->bottom_padding);
  }

  void show() override {
    this->fill_bounding_box();

    std::string location_text = format_location(this->model->location);
    location_text = elide_maybe(location_text);

    const auto label_pos_x = this->bounding_box.x + this->left_padding;
    const auto label_pos_y = this->bounding_box.y + this->top_padding;

    SetFont(this->font.get(), BLACK);

    DrawString(label_pos_x, label_pos_y, location_text.c_str());
  }

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override {
    if (event_type == EVT_POINTERUP) {
      this->edit_location();
      return 1;
    }
    return 0;
  }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<ifont> font;

  const int left_padding{50};
  const int top_padding{50};
  const int bottom_padding{25};

  void edit_location();

  std::string elide_maybe(const std::string &text) const {
    SetFont(this->font.get(), BLACK);
    const auto text_width = StringWidth(text.c_str());
    if (text_width <= this->bounding_box.w || text.size() <= 2) {
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
};

} // namespace taranis
