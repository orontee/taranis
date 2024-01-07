#pragma once

#include <memory>

#include "widget.h"

namespace taranis {

struct Paginated {
  typedef std::function<void()> PageChangedCallback;

  Paginated() {}

  virtual ~Paginated() {}

  virtual size_t page_count() const = 0;

  virtual size_t current_page() const = 0;

  void set_page_changed_callback(PageChangedCallback callback) {
    this->callback = callback;
  }

protected:
  PageChangedCallback callback;

  void on_page_changed();
  // expected to be called by implementations on page changes
};

struct DotPager : public Widget {

  DotPager(int pos_x, int pos_y, int width, int height,
           std::shared_ptr<Paginated> observed);

  void do_paint() override;

private:
  static constexpr int horizontal_padding{20};
  static constexpr int dot_radius{7};
  static constexpr int circle_thickness{3};

  std::shared_ptr<Paginated> observed;

  int dots_y;
};
} // namespace taranis
