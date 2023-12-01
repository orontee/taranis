#pragma once

#include <memory>
#include <string>

#include "fonts.h"
#include "model.h"
#include "widget.h"

namespace taranis {

class CurrentConditionBox : public Widget {
public:
  static constexpr int bottom_padding{25};

  CurrentConditionBox(int pos_x, int pos_y, std::shared_ptr<Model> model,
                      std::shared_ptr<Fonts> fonts);

  void do_paint() override;

private:
  static constexpr int horizontal_padding{50};

  std::shared_ptr<Model> model;
  std::shared_ptr<Fonts> fonts;

  static std::string get_description_text(const Condition &condition);

  std::string get_felt_temperature_text(const Condition &condition) const;
};

} // namespace taranis
