#pragma once

#include <inkview.h>
#include <memory>

#include "fonts.h"
#include "model.h"
#include "widget.h"

namespace taranis {

class StatusBar : public Widget {
public:
  StatusBar(std::shared_ptr<Model> model, std::shared_ptr<Fonts> fonts);

  void do_paint() override;

private:
  static constexpr int left_padding{50};
  static constexpr int right_padding{50};
  static constexpr int top_padding{25};
  static constexpr int bottom_padding{25};

  std::shared_ptr<Model> model;
  std::shared_ptr<ifont> font;
};

} // namespace taranis
