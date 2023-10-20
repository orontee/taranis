#pragma once

#include <ctime>
#include <inkview.h>
#include <memory>
#include <sstream>

#include "experimental/optional"
#include "fonts.h"
#include "model.h"
#include "util.h"
#include "widget.h"

namespace taranis {

class StatusBar : public Widget {
public:
  StatusBar(std::shared_ptr<Model> model, std::shared_ptr<Fonts> fonts)
      : model{model}, font{fonts->get_tiny_font()} {
    const auto height =
        2 * this->font->height + this->top_padding + this->bottom_padding;
    const auto width = ScreenWidth();
    const auto pos_x = 0;
    const auto pos_y = ScreenHeight() - height + 1;
    this->set_bounding_box(pos_x, pos_y, width, height);
  }

  void show() override {
    this->fill_bounding_box();

    std::stringstream first_row_text;
    if (this->model->refresh_date == std::experimental::nullopt) {
      first_row_text << GetLangText("Ongoing update…");
    } else {
      first_row_text << GetLangText("Last update:") << " "
                     << format_full_date(*this->model->refresh_date);
    }

    SetFont(this->font.get(), BLACK);
    const auto first_row_x = this->left_padding;
    const auto first_row_y = this->bounding_box.y + this->top_padding;
    DrawString(first_row_x, first_row_y, first_row_text.str().c_str());

    std::stringstream second_row_text;
    second_row_text << GetLangText("Weather data from") << " "
                    << this->model->source;

    SetFont(this->font.get(), DGRAY);
    const auto second_row_x = this->left_padding;
    const auto second_row_y = first_row_y + this->font->height;
    DrawString(second_row_x, second_row_y, second_row_text.str().c_str());
  }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<ifont> font;

  const int left_padding{50};
  const int top_padding{50};
  const int bottom_padding{25};
};

} // namespace taranis
