#pragma once

#include <inkview.h>

#include "keys.h"

namespace taranis {
struct Widget : public KeyEventConsumer {

  Widget() {}

  Widget(int pos_x, int pos_y) {
    this->bounding_box.x = pos_x;
    this->bounding_box.y = pos_y;
  }

  Widget(int pos_x, int pos_y, int width, int height) {
    this->bounding_box.x = pos_x;
    this->bounding_box.y = pos_y;
    this->bounding_box.w = width;
    this->bounding_box.h = height;
  }

  virtual ~Widget() {}

  const irect &get_bounding_box() const { return this->bounding_box; }

  int get_pos_x() const { return this->bounding_box.x; }

  void set_pos_x(int pos_x) { this->bounding_box.x = pos_x; }

  int get_pos_y() const { return this->bounding_box.y; }

  void set_pos_y(int pos_y) { this->bounding_box.y = pos_y; }

  int get_width() const { return this->bounding_box.w; }

  int get_height() const { return this->bounding_box.h; }

  virtual void show() = 0;

  void show_and_update() {
    this->show();
    PartialUpdate(this->bounding_box.x, this->bounding_box.y,
                  this->bounding_box.w, this->bounding_box.h);
  }

  bool is_in_bouding_box(int pos_x, int pos_y) const {
    return IsInRect(pos_x, pos_y, &this->bounding_box);
  }

  virtual int handle_pointer_event(int event_type, int pointer_pos_x,
                                   int pointer_pos_y) {
    // expect is_in_bouding_box(pointer_pos_x, pointer_pos_y) to be
    // true
    return 0;
  }

protected:
  irect bounding_box;

  void fill_bounding_box() const { FillAreaRect(&this->bounding_box, WHITE); }

  void set_bounding_box(int pos_x, int pos_y, int width, int height) {
    this->bounding_box.x = pos_x;
    this->bounding_box.y = pos_y;
    this->bounding_box.w = width;
    this->bounding_box.h = height;
  }

  void set_width(int width) { this->bounding_box.w = width; }

  void set_height(int height) { this->bounding_box.h = height; }
};
} // namespace taranis
