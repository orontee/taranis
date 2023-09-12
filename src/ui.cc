#include "ui.h"

#include "events.h"
#include "history.h"
#include "inkview.h"
#include "menu.h"
#include "model.h"

void taranis::handle_menu_item_selected(int item_index) {
  const auto event_handler = GetEventHandler();
  if (item_index == MENU_ITEM_REFRESH) {
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_requested, 0);
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
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::about_dialog_requested,
              0);
  } else if (MENU_ITEM_EMPTY_LOCATION_HISTORY < item_index and
             item_index <=
                 MENU_ITEM_EMPTY_LOCATION_HISTORY + LocationHistory::max_size) {
    const size_t history_index =
        item_index - MENU_ITEM_EMPTY_LOCATION_HISTORY - 1;
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::new_location_from_history,
              history_index);
  } else if (item_index == MENU_ITEM_QUIT) {
    SendEvent(event_handler, EVT_EXIT, 0, 0);
  }
}
