#pragma once

#include <memory>
#include <string>

#include "fonts.h"
#include "model.h"
#include "widget.h"

namespace taranis {

class CurrentConditionBox : public Widget {
public:
  CurrentConditionBox(int pos_x, int pos_y, int width,
                      std::shared_ptr<Model> model,
                      std::shared_ptr<Fonts> fonts);

  void do_paint() override;

private:
  static constexpr int bottom_padding{25};
  static constexpr int top_padding{0};
  static constexpr int left_padding{50};
  static constexpr int right_padding{25};

  static constexpr int inner_horizontal_spacing{50};
  static constexpr int inner_vertical_spacing{25};

  std::shared_ptr<Model> model;
  std::shared_ptr<Fonts> fonts;

  static std::string get_description_text(const Condition &condition);

  std::string get_felt_temperature_text(const Condition &condition) const;
};

} // namespace taranis
