#pragma once

#include <cmath>
#include <inkview.h>
#include <memory>
#include <vector>

#include "activatable.h"
#include "widget.h"

namespace taranis {

class Button : public Widget, Activatable {
public:
  Button(const int width, const int height, const ibitmap *const icon)
  : Widget{}, Activatable{}, icon{icon} {
    this->set_width(width);
    this->set_height(height);
  }

  void show() override {
    this->fill_bounding_box();

    const auto [pos_x, pos_y] = this->get_icon_top_left_position();
    DrawBitmap(pos_x, pos_y, this->icon);
  }

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override {
    if (event_type == EVT_POINTERDOWN) {
      this->activate();
      return 1;
    }

    if (event_type == EVT_POINTERDRAG) {
      this->desactivate();
      return 0;
    }

    if (event_type == EVT_POINTERUP) {
      this->desactivate();
      this->on_clicked();
      return 1;
    }
    return 0;
  }

  std::pair<int, int> get_icon_center_position() const {
    const int pos_x =
      this->bounding_box.x + this->bounding_box.w / 2;
    const int pos_y =
      this->bounding_box.y + this->bounding_box.h / 2;
    return {pos_x, pos_y};
  }

  std::pair<int, int> get_icon_top_left_position() const {
    const int pos_x = this->bounding_box.x +
                      (this->bounding_box.w - this->icon->width) / 2;
    const int pos_y = this->bounding_box.y +
                      (this->bounding_box.h - this->icon->height) / 2;
    return {pos_x, pos_y};
  }

protected :
  virtual void on_clicked() {};

private:
  const ibitmap *const icon;

  void on_activated_changed(bool activated) override {
    if (activated) {
      this->draw_inverted_icon();
    } else {
      this->show();
    }
    PartialUpdate(this->bounding_box.x, this->bounding_box.y,
                  this->bounding_box.w, this->bounding_box.h);
  }

  void draw_inverted_icon() const {
    const auto [pos_x, pos_y] = this->get_icon_center_position();
    const int radius = std::max((this->bounding_box.w + this->icon->width) / 4,
				(this->bounding_box.h + this->icon->height) / 4);
    auto canvas = GetCanvas();
    invertCircle(pos_x, pos_y, radius, canvas);
  }
};
}
