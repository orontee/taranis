#include "pager.h"
#include "inkview.h"

namespace taranis {

void Paginated::on_page_changed() {
  if (this->callback) {
    this->callback();
  }
}

DotPager::DotPager(int pos_x, int pos_y, int width, int height,
                   std::shared_ptr<Paginated> observed)
    : Widget{pos_x, pos_y, width, height}, observed{observed},
      dots_y{this->bounding_box.y + this->bounding_box.h / 2} {}

void DotPager::do_paint() {
  const auto dots_count = this->observed->page_count();
  if (dots_count == 0) {
    return;
  }

  auto dots_width = DotPager::dot_radius * dots_count;
  if (dots_count > 0) {
    dots_width += DotPager::horizontal_padding * (dots_count - 1);
  }
  if (dots_width > this->bounding_box.w) {
    BOOST_LOG_TRIVIAL(warning) << "Too many pages for dot pager width!";
  }

  const int first_dot_x = this->bounding_box.x +
                          (this->bounding_box.w - dots_width) / 2 +
                          DotPager::dot_radius;
  const auto current_page = this->observed->current_page();
  for (size_t dot_index = 0; dot_index < dots_count; ++dot_index) {
    const auto dot_x = first_dot_x + dot_index * (2 * DotPager::dot_radius +
                                                  DotPager::horizontal_padding);
    if (dot_index == current_page) {
      DrawCircleQuarter(dot_x, this->dots_y, DotPager::dot_radius,
                        ROUND_ALL | ROUND_FILL_INSIDE,
                        DotPager::circle_thickness, BLACK, WHITE);
    } else {
      DrawCircleQuarter(dot_x, this->dots_y, DotPager::dot_radius, ROUND_ALL,
                        DotPager::circle_thickness, BLACK, WHITE);
    }
  }
}
} // namespace taranis
