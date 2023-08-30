#pragma once

#include <inkview.h>
#include <memory>
#include <sstream>

#include "fonts.h"
#include "model.h"
#include "widget.h"

namespace taranis {

class LocationBox : public Widget {
public:
  LocationBox(int pos_x, int pos_y, std::shared_ptr<Model> model,
              std::shared_ptr<Fonts> fonts)
      : Widget{pos_x, pos_y}, model{model}, font{fonts->get_bold_font()} {
    this->set_width(4 * ScreenWidth() / 5);
    this->set_height(this->font->height + this->top_padding +
                     this->bottom_padding);
  }

  virtual void show() {
    this->fill_bounding_box();

    std::stringstream text;
    text << this->model->location.town << ", " << this->model->location.country;

    // TODO elide to box width

    const auto label_pos_x = this->bounding_box.x + this->left_padding;
    const auto label_pos_y = this->bounding_box.y + this->top_padding;

    SetFont(this->font.get(), BLACK);
    DrawString(label_pos_x, label_pos_y, text.str().c_str());
  }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<ifont> font;

  const int left_padding{50};
  const int top_padding{50};
  const int bottom_padding{25};
};

} // namespace taranis
