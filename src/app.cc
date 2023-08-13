#include "app.h"

void taranis::handle_menu_item_selected(int index) {
  const auto event_handler = GetEventHandler();
  switch (index) {
  case MENU_ITEM_REFRESH:
    SendEvent(event_handler, EVT_CUSTOM, APP_EVT_REFRESH_REQUESTED, 0);
    break;
  case MENU_ITEM_ABOUT:
    break;
  case MENU_ITEM_QUIT:
    SendEvent(event_handler, EVT_EXIT, 0, 0);
  }
}
