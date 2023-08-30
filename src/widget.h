#pragma once

#include <inkview.h>

namespace taranis {
struct Widget {

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

  irect get_bounding_box() const { return this->bounding_box; }

  int get_pos_x() { return this->bounding_box.x; }

  int get_pos_y() { return this->bounding_box.y; }

  int get_width() { return this->bounding_box.w; }

  int get_height() { return this->bounding_box.h; }

  virtual void show() = 0;

protected:
  irect bounding_box;

  void fill_bounding_box() { FillAreaRect(&this->bounding_box, WHITE); }

  void set_bounding_box(int pos_x, int pos_y, int width, int height) {
    this->bounding_box.x = pos_x;
    this->bounding_box.y = pos_y;
    this->bounding_box.w = width;
    this->bounding_box.h = height;
  }

  void set_pos_x(int pos_x) { this->bounding_box.x = pos_x; }

  void set_pos_y(int pos_y) { this->bounding_box.y = pos_y; }

  void set_width(int width) { this->bounding_box.w = width; }

  void set_height(int height) { this->bounding_box.h = height; }
};
} // namespace taranis
