#pragma once

#include <experimental/optional>
#include <inkview.h>
#include <memory>

#include "button.h"
#include "fonts.h"
#include "icons.h"
#include "model.h"

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {

void alert_dialog_handler(int button_index);

class AlertsButton : public Button {
public:
  AlertsButton(std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
               std::shared_ptr<Fonts> fonts)
      : Button{AlertsButton::icon_size + 2 * AlertsButton::padding,
               AlertsButton::icon_size + 2 * AlertsButton::padding,
               BitmapStretchProportionally(icons->get("warning"),
                                           AlertsButton::icon_size,
                                           AlertsButton::icon_size)},
        model{model}, font{fonts->get_normal_font()} {}

  void show() override {
    if (this->model->alerts.empty()) {
      this->fill_bounding_box();
    } else {
      Button::show();
    }
  }

  void open_dialog_maybe();

  static const int padding{50};
  static const int icon_size{70};

protected:
  void on_clicked() override {
    if (this->model->alerts.empty()) {
      return;
    }

    this->open_dialog_maybe();
  }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<ifont> font;
};
} // namespace taranis
