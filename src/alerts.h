#pragma once

#include <inkview.h>
#include <memory>

#include "button.h"
#include "fonts.h"
#include "icons.h"
#include "model.h"

namespace taranis {

class AlertsButton : public Button {
public:
  AlertsButton(int icon_size, std::shared_ptr<Model> model,
               std::shared_ptr<Icons> icons, std::shared_ptr<Fonts> fonts)
      : Button{icon_size, icons->get("warning")}, model{model},
        font{fonts->get_normal_font()} {}

  bool is_visible() const override { return not this->model->alerts.empty(); }

  void open_dialog_maybe();

protected:
  void on_clicked() override { this->open_dialog_maybe(); }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<ifont> font;

  static void alert_dialog_handler(int button_index);
};
} // namespace taranis
