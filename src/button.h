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
  Button(const int icon_size, const ibitmap *const icon,
         const ibitmap *const icon_disabled = nullptr)
      : Widget{}, Activatable{}, icon{BitmapStretchProportionally(
                                     icon, icon_size, icon_size)},
        icon_disabled{
            BitmapStretchProportionally(icon_disabled, icon_size, icon_size)} {
    this->set_width(icon_size * std::sqrt(2));
    this->set_height(icon_size * std::sqrt(2));
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
    const int pos_x = this->bounding_box.x + this->bounding_box.w / 2;
    const int pos_y = this->bounding_box.y + this->bounding_box.h / 2;
    return {pos_x, pos_y};
  }

  std::pair<int, int> get_icon_top_left_position() const {
    const int pos_x =
        this->bounding_box.x + (this->bounding_box.w - this->icon->width) / 2;
    const int pos_y =
        this->bounding_box.y + (this->bounding_box.h - this->icon->height) / 2;
    return {pos_x, pos_y};
  }

  void do_paint() override {
    const auto [pos_x, pos_y] = this->get_icon_top_left_position();
    const auto *const icon_to_draw =
        this->is_enabled() ? this->icon.get() : this->icon_disabled.get();
    if (icon_to_draw) {
      DrawBitmap(pos_x, pos_y, icon_to_draw);
    }
  }

protected:
  virtual void on_clicked(){};

private:
  std::unique_ptr<ibitmap> icon;
  std::unique_ptr<ibitmap> icon_disabled;

  void on_activated_changed(bool activated) override {
    if (activated) {
      this->draw_inverted_icon();
    } else {
      this->paint();
    }
    PartialUpdate(this->bounding_box.x, this->bounding_box.y,
                  this->bounding_box.w, this->bounding_box.h);
  }

  void draw_inverted_icon() const {
    const auto [pos_x, pos_y] = this->get_icon_center_position();
    const int radius =
        static_cast<int>(std::sqrt(2) *
                         std::max(this->icon->width, this->icon->height) / 2) -
        2;
    auto canvas = GetCanvas();
    invertCircle(pos_x, pos_y, radius, canvas);
  }
};
} // namespace taranis
