#pragma once

#include <memory>

#include "fonts.h"
#include "model.h"
#include "widget.h"

using namespace std::string_literals;

namespace taranis {

class LocationBox : public Widget {
public:
  LocationBox(int pos_x, int pos_y, int width, std::shared_ptr<Model> model,
              std::shared_ptr<Fonts> fonts);

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override;

  void edit_location();

  void do_paint() override;

private:
  static constexpr int left_padding{50};
  static constexpr int right_padding{0};
  static constexpr int top_padding{50};
  static constexpr int bottom_padding{25};

  std::shared_ptr<Model> model;
  std::shared_ptr<ifont> font;

  static void keyboard_handler(char *text);
};

} // namespace taranis
