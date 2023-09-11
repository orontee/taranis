#pragma once

#include <inkview.h>
#include <memory>
#include <vector>

#include "alerts.h"
#include "currentconditionbox.h"
#include "fonts.h"
#include "hourlyforecastbox.h"
#include "icons.h"
#include "locationbox.h"
#include "menu.h"
#include "model.h"
#include "statusbar.h"

namespace taranis {

void handle_menu_item_selected(int item_index);

class Ui {
public:
  // ⚠️ Must be instantiated after application received the EVT_INIT
  // event otherwise opening fonts, etc. fails

  Ui(std::shared_ptr<Model> model)
      : model{model}, icons{new Icons{}}, fonts{new Fonts{}} {
    SetPanelType(0);
    SetOrientation(0);

    auto location_box =
        std::make_shared<LocationBox>(0, 0, this->model, this->fonts);

    auto current_condition_box = std::make_shared<CurrentConditionBox>(
        0, location_box->get_height(), this->model, this->fonts);

    auto status_bar = std::make_shared<StatusBar>(this->model, this->fonts);

    const auto remaining_height = ScreenHeight() - location_box->get_height() -
                                  current_condition_box->get_height() -
                                  status_bar->get_height();

    auto hourly_forecast_box = std::make_shared<HourlyForecastBox>(
        0,
        current_condition_box->get_pos_y() +
            current_condition_box->get_height(),
        ScreenWidth(), remaining_height, this->model, this->icons, this->fonts);

    auto menu_button =
        std::make_shared<MenuButton>(this->model, this->icons, this->fonts);
    menu_button->set_menu_handler(handle_menu_item_selected);

    const auto &menu_button_bounding_box = menu_button->get_bounding_box();

    this->alerts_button =
        std::make_shared<AlertsButton>(model, this->icons, this->fonts);
    this->alerts_button->set_pos_x(menu_button_bounding_box.x);
    this->alerts_button->set_pos_y(menu_button_bounding_box.y +
                                   menu_button_bounding_box.h);

    this->children.push_back(location_box);
    this->children.push_back(menu_button);
    this->children.push_back(alerts_button);
    this->children.push_back(current_condition_box);
    this->children.push_back(hourly_forecast_box);
    this->children.push_back(status_bar);
  }

  void show() {
    ClearScreen();

    for (auto widget : this->children) {
      widget->show();
    }
    FullUpdate();
  }

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) {
    for (auto widget : this->children) {
      if (Ui::is_on_widget(pointer_pos_x, pointer_pos_y, widget)) {
        return widget->handle_pointer_event(event_type, pointer_pos_x,
                                            pointer_pos_y);
      }
    }
    return 0;
  }

  int handle_key_pressed(int key) {
    if (key == IV_KEY_HOME) {
      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_EXIT, 0, 0);
      return 1;
    }

    for (auto widget : this->children) {
      const auto handled = widget->handle_key_pressed(key);
      if (handled) {
        return 1;
      }
    }
    return 0;
  }

  void display_alert() { this->alerts_button->open_dialog_maybe(); }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  std::shared_ptr<AlertsButton> alerts_button;

  std::vector<std::shared_ptr<Widget>> children;

  static bool is_on_widget(int pointer_pos_x, int pointer_pos_y,
                           std::shared_ptr<Widget> widget) {
    if (not widget)
      return false;

    return widget->is_in_bouding_box(pointer_pos_x, pointer_pos_y);
  }
};

} // namespace taranis
