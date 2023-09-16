#pragma once

#include <inkview.h>
#include <memory>
#include <vector>

#include "alerts.h"
#include "currentconditionbox.h"
#include "dailyforecastbox.h"
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

    const auto remaining_height =
        status_bar->get_pos_y() - (current_condition_box->get_pos_y() +
                                   current_condition_box->get_height());

    this->hourly_forecast_box = std::make_shared<HourlyForecastBox>(
        0,
        current_condition_box->get_pos_y() +
            current_condition_box->get_height(),
        ScreenWidth(), remaining_height, this->model, this->icons, this->fonts);

    this->daily_forecast_box = std::make_shared<DailyForecastBox>(
        0,
        current_condition_box->get_pos_y() +
            current_condition_box->get_height(),
        ScreenWidth(), remaining_height, this->model, this->icons, this->fonts);

    auto menu_button =
        std::make_shared<MenuButton>(this->model, this->icons, this->fonts);
    menu_button->set_pos_x(ScreenWidth() - menu_button->get_width() -
                           Ui::button_margin);
    menu_button->set_pos_y(Ui::button_margin);
    menu_button->set_menu_handler(handle_menu_item_selected);

    const auto &current_condition_bounding_box =
        current_condition_box->get_bounding_box();

    this->alerts_button = std::make_shared<AlertsButton>(
        Ui::alert_icon_size, model, this->icons, this->fonts);
    this->alerts_button->set_pos_x(ScreenWidth() - alerts_button->get_width() -
                                   Ui::button_margin);
    this->alerts_button->set_pos_y(current_condition_bounding_box.y +
                                   current_condition_bounding_box.h / 2 -
                                   this->alerts_button->get_height() / 2);

    this->children.push_back(location_box);
    this->children.push_back(menu_button);
    this->children.push_back(current_condition_box);
    this->children.push_back(alerts_button);
    this->children.push_back(status_bar);
  }

  void show() {
    ClearScreen();

    this->select_middle_widget();

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

  std::shared_ptr<Widget> hourly_forecast_box;
  std::shared_ptr<Widget> daily_forecast_box;

  std::vector<std::shared_ptr<Widget>> children;

  const int alert_icon_size = 150;
  const int button_margin = 25;

  static bool is_on_widget(int pointer_pos_x, int pointer_pos_y,
                           std::shared_ptr<Widget> widget) {
    if (not widget)
      return false;

    return widget->is_in_bouding_box(pointer_pos_x, pointer_pos_y);
  }

  void select_middle_widget() {
    auto widget_to_display = this->model->display_daily_forecast
                                 ? this->daily_forecast_box
                                 : this->hourly_forecast_box;
    auto widget_to_hide = not this->model->display_daily_forecast
                              ? this->daily_forecast_box
                              : this->hourly_forecast_box;
    if (this->children.back() == widget_to_hide) {
      this->children.pop_back();
    }
    if (this->children.back() != widget_to_display) {
      this->children.push_back(widget_to_display);
    }
  }
};

} // namespace taranis
