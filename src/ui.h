#pragma once

#include <inkview.h>
#include <iomanip>
#include <memory>
#include <sstream>

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

void handle_current_condition_dialog_button_clicked(int button_index);

class Ui {
public:
  // ⚠️ Must be instantiated after application received the EVT_INIT
  // event otherwise opening fonts, etc. fails

  Ui(std::shared_ptr<Model> model)
      : model{model}, icons{new Icons{}}, fonts{new Fonts{}} {
    SetPanelType(0);
    SetOrientation(0);

    this->location_box =
        std::make_shared<LocationBox>(0, 0, this->model, this->fonts);

    this->current_condition_box = std::make_shared<CurrentConditionBox>(
        0, this->location_box->get_height(), this->model, this->fonts);

    this->status_bar = std::make_shared<StatusBar>(this->model, this->fonts);

    const auto remaining_height = ScreenHeight() -
                                  this->location_box->get_height() -
                                  this->current_condition_box->get_height() -
                                  this->status_bar->get_height();

    this->hourly_forecast_box = std::make_shared<HourlyForecastBox>(
        0,
        this->current_condition_box->get_pos_y() +
            this->current_condition_box->get_height(),
        ScreenWidth(), remaining_height, this->model, this->icons, this->fonts);

    this->menu_button = std::make_shared<MenuButton>(this->icons, this->fonts);
  }

  void show() {
    ClearScreen();

    this->location_box->show();
    this->menu_button->show();
    this->current_condition_box->show();
    this->hourly_forecast_box->show();
    this->status_bar->show();

    FullUpdate();
  }

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) {
    if (event_type == EVT_POINTERDOWN) {
      if (this->activate_menu_button_maybe(pointer_pos_x, pointer_pos_y)) {
        return 1;
      }
    }

    if (event_type == EVT_POINTERDRAG) {
      this->desactivate_menu_button();
      return 0;
    }

    if (event_type == EVT_POINTERUP) {
      if (this->is_pointer_on_menu_button(pointer_pos_x, pointer_pos_y)) {
        this->open_menu();
        return 1;
      }

      if (this->is_pointer_on_current_condition_box(pointer_pos_x,
                                                    pointer_pos_y)) {
        this->open_current_condition_dialog();
        return 1;
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

    if (key == IV_KEY_MENU) {
      this->open_menu();
      return 1;
    }

    if (key == IV_KEY_PREV) {
      this->hourly_forecast_box->decrease_forecast_offset();
      return 1;
    }

    if (key == IV_KEY_NEXT) {
      this->hourly_forecast_box->increase_forecast_offset();
      return 1;
    }

    return 0;
  }

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  std::shared_ptr<LocationBox> location_box;
  std::shared_ptr<CurrentConditionBox> current_condition_box;
  std::shared_ptr<HourlyForecastBox> hourly_forecast_box;
  std::shared_ptr<StatusBar> status_bar;
  std::shared_ptr<MenuButton> menu_button;

  bool activate_menu_button_maybe(int pointer_pos_x, int pointer_pos_y) {
    const auto menu_button_bounding_box = this->menu_button->get_bounding_box();

    if (IsInRect(pointer_pos_x, pointer_pos_y, &menu_button_bounding_box)) {
      this->menu_button->activate();
      return true;
    }
    return false;
  }

  void desactivate_menu_button() { this->menu_button->desactivate(); }

  bool is_pointer_on_menu_button(int pointer_pos_x, int pointer_pos_y) {
    const auto menu_button_bounding_box = this->menu_button->get_bounding_box();

    return IsInRect(pointer_pos_x, pointer_pos_y, &menu_button_bounding_box);
  }

  void open_menu() {
    if (this->menu_button->is_activated()) {
      this->menu_button->desactivate();
    }
    this->menu_button->open_menu(handle_menu_item_selected);
  }

  bool is_pointer_on_current_condition_box(int pointer_pos_x,
                                           int pointer_pos_y) {
    const auto bounding_box = this->current_condition_box->get_bounding_box();
    return IsInRect(pointer_pos_x, pointer_pos_y, &bounding_box);
  }

  void open_current_condition_dialog() {
    if (not this->model->current_condition) {
      return;
    }
    const auto condition = *(this->model->current_condition);

    const char *const time_format = "%H:%M";
    std::string sunrise_text{"?????"};
    std::strftime(const_cast<char *>(sunrise_text.c_str()), 6, time_format,
                  std::localtime(&condition.sunrise));
    std::string sunset_text{"?????"};
    std::strftime(const_cast<char *>(sunset_text.c_str()), 6, time_format,
                  std::localtime(&condition.sunset));

    std::stringstream content;
    content << T("Sunrise") << std::right << std::setw(10) << sunrise_text
            << std::endl
            << T("Sunset") << std::right << std::setw(10) << sunset_text
            << std::endl
            << T("Pressure") << std::right << std::setw(10)
            << std::to_string(condition.pressure) + "hPa" << std::endl
            << T("Humidity") << std::right << std::setw(10)
            << std::to_string(condition.humidity) + "%" << std::endl
            << T("UV index") << std::right << std::setw(10) << std::fixed
            << std::setprecision(1) << condition.uv_index << std::endl
            << T("Visibility") << std::right << std::setw(10)
            << std::to_string(condition.visibility) + "m" << std::endl
            << T("Wind") << std::right << std::setw(10)
            << std::to_string(static_cast<int>(condition.wind_speed)) + "m/s"
            << std::endl
            << T("Gust") << std::right << std::setw(10)
            << std::to_string(static_cast<int>(condition.wind_gust)) + "m/s"
            << std::endl;

    Dialog(ICON_INFORMATION, T("Current Weather Conditions"),
           content.str().c_str(), T("Ok"), nullptr,
           &handle_current_condition_dialog_button_clicked);
  }
};

} // namespace taranis
