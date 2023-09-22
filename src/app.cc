#include "app.h"
#include "events.h"
#include "inkview.h"

void taranis::handle_about_dialog_button_clicked(int button_index) {
  CloseDialog();
}

void taranis::refresh_data() {
  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data, 0);
}
