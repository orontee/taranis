#include "ui.h"

#include "boost/log/trivial.hpp"
#include "events.h"
#include "history.h"
#include "inkview.h"
#include "logging.h"
#include "menu.h"
#include "model.h"

void taranis::handle_menu_item_selected(int item_index) {
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
    BOOST_LOG_TRIVIAL(error) << "Unexpected item index" << item_index;
  }
}
