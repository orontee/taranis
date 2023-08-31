#include "ui.h"

#include "events.h"

void taranis::handle_menu_item_selected(int item_index) {
  const auto event_handler = GetEventHandler();
  switch (item_index) {
  case MENU_ITEM_REFRESH:
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_requested, 0);
    break;
  case MENU_ITEM_ABOUT:
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::about_dialog_requested,
              0);
    break;
  case MENU_ITEM_QUIT:
    SendEvent(event_handler, EVT_EXIT, 0, 0);
    break;
  }
}

void taranis::handle_current_condition_dialog_button_clicked(int button_index) {
  CloseDialog();
}
