#include "ui.h"

#include <boost/log/trivial.hpp>
#include <inkview.h>
#include <memory>

#include "currentconditionbox.h"
#include "events.h"
#include "history.h"
#include "keys.h"
#include "locationbox.h"
#include "locationselector.h"
#include "logo.h"
#include "menu.h"
#include "model.h"
#include "statusbar.h"

namespace taranis {

Ui::Ui(std::shared_ptr<Config> config, std::shared_ptr<Model> model)
    : KeyEventDispatcher{}, config{config}, model{model}, icons{new Icons{}},
      fonts{new Fonts{}} {
  SetPanelType(PANEL_DISABLED);
  SetOrientation(0);

  auto menu_button = std::make_shared<MenuButton>(
      Ui::menu_button_icon_size, this->model, this->icons, this->fonts);
  menu_button->set_pos_x(ScreenWidth() - menu_button->get_width() -
                         Ui::button_margin);
  menu_button->set_pos_y(Ui::button_margin);
  menu_button->set_menu_handler(&handle_menu_item_selected);

  this->alert_viewer = std::make_shared<AlertViewer>(model, this->fonts);

  auto alerts_button = std::make_shared<AlertsButton>(
      Ui::alert_button_icon_size, model, this->icons, this->alert_viewer);
  alerts_button->set_pos_x(menu_button->get_pos_x() -
                           alerts_button->get_width() - Ui::button_margin);
  alerts_button->set_pos_y(Ui::button_margin);

  this->location_box = std::make_shared<LocationBox>(
      0, 0, alerts_button->get_pos_x() - Ui::button_margin, this->model,
      this->fonts);

  auto current_condition_box = std::make_shared<CurrentConditionBox>(
      0, this->location_box->get_height(), ScreenWidth(), this->model,
      this->fonts);

  auto status_bar = std::make_shared<StatusBar>(this->model, this->fonts);

  const auto remaining_height = status_bar->get_pos_y() -
                                (current_condition_box->get_pos_y() +
                                 current_condition_box->get_height()) -
                                1;

  this->forecast_stack = std::make_shared<ForecastStack>(
      0,
      current_condition_box->get_pos_y() + current_condition_box->get_height(),
      ScreenWidth(), remaining_height, this->model, this->icons, this->fonts);

  this->location_selector = std::make_shared<LocationSelector>(
      this->location_selector_icon_size, this->fonts, this->icons);

  this->modals.push_back(this->alert_viewer);

  this->children.push_back(this->location_box);
  this->children.push_back(alerts_button);
  this->children.push_back(menu_button);
  this->children.push_back(current_condition_box);
  this->children.push_back(this->forecast_stack);
  this->children.push_back(status_bar);

  this->register_key_event_consumer(this->alert_viewer);
  this->register_key_event_consumer(menu_button);
  this->register_key_event_consumer(this->forecast_stack);
}

void Ui::paint() {
  ClearScreen();
  // Will justify to call do_paint() on children and not paint()

  this->check_modal_visibility();

  if (this->visible_modal) {
    BOOST_LOG_TRIVIAL(debug) << "Painting visible modal";
    this->visible_modal->paint();
  } else {
    for (auto widget : this->children) {
      if (widget->is_visible()) {
        widget->do_paint();
      }
    }
  }
  FullUpdate();
}

int Ui::handle_pointer_event(int event_type, int pointer_pos_x,
                             int pointer_pos_y) {
  if (this->visible_modal) {
    return this->visible_modal->handle_pointer_event(event_type, pointer_pos_x,
                                                     pointer_pos_y);
  } else {
    for (auto widget : this->children) {
      if (Ui::is_on_widget(pointer_pos_x, pointer_pos_y, widget) and
          widget->is_enabled()) {
        return widget->handle_pointer_event(event_type, pointer_pos_x,
                                            pointer_pos_y);
      }
    }
  }
  return 0;
}

bool Ui::is_key_event_consumer_active(
    std::shared_ptr<KeyEventConsumer> consumer) {
  if (this->visible_modal) {
    return consumer == this->visible_modal;
  }
  return KeyEventDispatcher::is_key_event_consumer_active(consumer);
}

void Ui::switch_forecast_widget() { this->forecast_stack->switch_forecast(); }

void Ui::open_location_list(const std::vector<Location> &locations) {
  this->location_selector->set_locations(locations);
  this->location_selector->open();
}

std::optional<Location>
Ui::get_location_from_location_list(size_t index) const {
  return this->location_selector->get_location(index);
}

void Ui::generate_logo_maybe() const {
  LogoGenerator generator{this->config, this->model, this->icons, this->fonts};
  generator.generate_maybe();
}

bool Ui::is_on_widget(int pointer_pos_x, int pointer_pos_y,
                      std::shared_ptr<Widget> widget) {
  if (not widget)
    return false;

  return widget->is_in_bouding_box(pointer_pos_x, pointer_pos_y);
}

void Ui::check_modal_visibility() {
  const auto found_visible_modal =
      std::find_if(this->modals.begin(), this->modals.end(),
                   [](const auto &modal) { return modal->is_visible(); });

  if (found_visible_modal != this->modals.end()) {
    if (this->visible_modal != *found_visible_modal) {
      BOOST_LOG_TRIVIAL(debug) << "Found visible modal";
      this->visible_modal = *found_visible_modal;
      this->register_key_event_consumer(this->visible_modal);
    }
  } else {
    if (this->visible_modal) {
      BOOST_LOG_TRIVIAL(debug) << "Modal hidden";
      this->unregister_key_event_consumer(this->visible_modal);
      this->visible_modal = nullptr;
    }
  }
}

void Ui::handle_menu_item_selected(int item_index) {
  BOOST_LOG_TRIVIAL(debug) << "Handling menu item selected " << item_index;

  const auto event_handler = GetEventHandler();
  if (item_index == MENU_ITEM_REFRESH) {
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data,
              CallContext::triggered_by_user);
  } else if (item_index == MENU_ITEM_EDIT_LOCATION) {
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::edit_location, 0);
  } else if (item_index == MENU_ITEM_TOGGLE_FAVORITE_LOCATION) {
    SendEvent(event_handler, EVT_CUSTOM,
              CustomEvent::toggle_current_location_favorite, 0);
  } else if (item_index == MENU_ITEM_CONFIGURE) {
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::open_config_editor, 0);
  } else if (MENU_ITEM_EMPTY_LOCATION_HISTORY < item_index and
             item_index <= MENU_ITEM_EMPTY_LOCATION_HISTORY +
                               LocationHistoryProxy::max_size) {
    const size_t history_index =
        item_index - MENU_ITEM_EMPTY_LOCATION_HISTORY - 1;
    SendEvent(event_handler, EVT_CUSTOM,
              CustomEvent::select_location_from_history, history_index);
  } else if (item_index == MENU_ITEM_QUIT) {
    CloseApp();
  } else if (item_index == -1) {
    BOOST_LOG_TRIVIAL(debug) << "Menu to be closed";
  } else {
    BOOST_LOG_TRIVIAL(error) << "Unexpected item index " << item_index;
  }
}

} // namespace taranis
