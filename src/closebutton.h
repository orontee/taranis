#pragma once

#include <memory>

#include "button.h"
#include "icons.h"

namespace taranis {

class CloseButton : public Button {
public:
  CloseButton(int icon_size, std::shared_ptr<Icons> icons)
      : Button{icon_size, icons->get("close")} {}

  typedef std::function<void()> ClickHandler;

  void set_click_handler(ClickHandler handler) {
    this->click_handler = handler;
  }

protected:
  void on_clicked() override {
    if (!this->click_handler) {
      return;
    }
    this->click_handler();
  }

private:
  ClickHandler click_handler;
};
} // namespace taranis
