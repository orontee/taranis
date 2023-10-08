#include "ui.h"

#include <boost/log/trivial.hpp>
#include <inkview.h>

#include "currentconditionbox.h"
#include "dailyforecastbox.h"
#include "events.h"
#include "history.h"
#include "locationbox.h"
#include "menu.h"
#include "model.h"
#include "statusbar.h"

namespace taranis {

Ui::Ui(std::shared_ptr<Model> model)
    : model{model}, icons{new Icons{}}, fonts{new Fonts{}} {
  SetPanelType(0);
  SetOrientation(0);

  auto location_box =
      std::make_shared<LocationBox>(0, 0, this->model, this->fonts);

  auto current_condition_box = std::make_shared<CurrentConditionBox>(
      0, location_box->get_height(), this->model, this->fonts);

  auto status_bar = std::make_shared<StatusBar>(this->model, this->fonts);

  const auto remaining_height = status_bar->get_pos_y() -
                                (current_condition_box->get_pos_y() +
                                 current_condition_box->get_height()) -
                                1;

  this->hourly_forecast_box = std::make_shared<HourlyForecastBox>(
      0,
      current_condition_box->get_pos_y() + current_condition_box->get_height(),
      ScreenWidth(), remaining_height, this->model, this->icons, this->fonts);

  this->daily_forecast_box = std::make_shared<DailyForecastBox>(
      0,
      current_condition_box->get_pos_y() + current_condition_box->get_height(),
      ScreenWidth(), remaining_height, this->model, this->icons, this->fonts);

  auto menu_button =
      std::make_shared<MenuButton>(this->model, this->icons, this->fonts);
  menu_button->set_pos_x(ScreenWidth() - menu_button->get_width() -
                         Ui::button_margin);
  menu_button->set_pos_y(Ui::button_margin);
  menu_button->set_menu_handler(&handle_menu_item_selected);

  const auto &current_condition_bounding_box =
      current_condition_box->get_bounding_box();

  this->alerts_button = std::make_shared<AlertsButton>(
      Ui::alert_icon_size, model, this->icons, this->fonts);
  this->alerts_button->set_pos_x(ScreenWidth() - alerts_button->get_width() -
                                 Ui::button_margin);
  this->alerts_button->set_pos_y(current_condition_bounding_box.y +
                                 current_condition_bounding_box.h / 2 -
                                 this->alerts_button->get_height() / 2 -
                                 CurrentConditionBox::bottom_padding);

  this->children.push_back(location_box);
  this->children.push_back(menu_button);
  this->children.push_back(current_condition_box);
  this->children.push_back(this->alerts_button);
  this->children.push_back(status_bar);

  this->swipe_detector.set_bounding_box(
      this->hourly_forecast_box->get_bounding_box());
}

void Ui::show() {
  ClearScreen();

  this->select_forecast_widget();

  for (auto widget : this->children) {
    widget->show();
  }
  FullUpdate();
}

int Ui::handle_pointer_event(int event_type, int pointer_pos_x,
                             int pointer_pos_y) {
  if (this->handle_possible_swipe(event_type, pointer_pos_x, pointer_pos_y)) {
    return 1;
  }
  for (auto widget : this->children) {
    if (Ui::is_on_widget(pointer_pos_x, pointer_pos_y, widget)) {
      return widget->handle_pointer_event(event_type, pointer_pos_x,
                                          pointer_pos_y);
    }
  }
  return 0;
}

int Ui::handle_key_pressed(int key) {
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

void Ui::switch_forecast_widget() {
  this->select_forecast_widget();
  auto forecast_widget = this->get_forecast_widget();
  if (forecast_widget) {
    forecast_widget->show_and_update();
  }
}

bool Ui::is_on_widget(int pointer_pos_x, int pointer_pos_y,
                      std::shared_ptr<Widget> widget) {
  if (not widget)
    return false;

  return widget->is_in_bouding_box(pointer_pos_x, pointer_pos_y);
}

std::shared_ptr<Widget> Ui::get_forecast_widget() const {
  auto forecast_widget = this->model->display_daily_forecast
                             ? this->daily_forecast_box
                             : this->hourly_forecast_box;
  return forecast_widget;
}

void Ui::select_forecast_widget() {
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

int Ui::handle_possible_swipe(int event_type, int pointer_pos_x,
                              int pointer_pos_y) {
  const auto swipe = this->swipe_detector.guess_event_swipe_type(
      event_type, pointer_pos_x, pointer_pos_y);
  if (swipe != SwipeType::no_swipe) {
    auto current_forecast_widget = this->get_forecast_widget();
    if (current_forecast_widget == this->daily_forecast_box) {
      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM,
                CustomEvent::change_daily_forecast_display, 0);
      return 1;
    } else if (current_forecast_widget == this->hourly_forecast_box) {
      if (swipe == SwipeType::left_swipe) {
        this->hourly_forecast_box->increase_forecast_offset();
        return 1;
      } else if (swipe == SwipeType::right_swipe) {
        this->hourly_forecast_box->decrease_forecast_offset();
        return 1;
      }
    }
  }
  return 0;
}

void Ui::handle_menu_item_selected(int item_index) {
  BOOST_LOG_TRIVIAL(debug) << "Handling menu item selected " << item_index;

  const auto event_handler = GetEventHandler();
  if (item_index == MENU_ITEM_REFRESH) {
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data, 0);
  } else if (item_index == MENU_ITEM_TOGGLE_FAVORITE_LOCATION) {
    SendEvent(event_handler, EVT_CUSTOM,
              CustomEvent::toggle_current_location_favorite, 0);
  } else if (item_index == MENU_ITEM_UNIT_SYSTEM_STANDARD) {
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::change_unit_system,
              UnitSystem::standard);
  } else if (item_index == MENU_ITEM_UNIT_SYSTEM_METRIC) {
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::change_unit_system,
              UnitSystem::metric);
  } else if (item_index == MENU_ITEM_UNIT_SYSTEM_IMPERIAL) {
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::change_unit_system,
              UnitSystem::imperial);
  } else if (item_index == MENU_ITEM_ABOUT) {
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::show_about_dialog, 0);
  } else if (MENU_ITEM_EMPTY_LOCATION_HISTORY < item_index and
             item_index <= MENU_ITEM_EMPTY_LOCATION_HISTORY +
                               LocationHistoryProxy::max_size) {
    const size_t history_index =
        item_index - MENU_ITEM_EMPTY_LOCATION_HISTORY - 1;
    SendEvent(event_handler, EVT_CUSTOM,
              CustomEvent::select_location_from_history, history_index);
  } else if (item_index == MENU_ITEM_QUIT) {
    SendEvent(event_handler, EVT_EXIT, 0, 0);
  } else {
    BOOST_LOG_TRIVIAL(error) << "Unexpected item index " << item_index;
  }
}

} // namespace taranis
