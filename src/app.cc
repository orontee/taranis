#include "app.h"
#include "events.h"
#include "inkview.h"

void taranis::handle_about_dialog_button_clicked(int button_index) {
  CloseDialog();
}

void taranis::App::refresh_data_maybe() {
  iv_netinfo *netinfo = NetInfo();
  if (netinfo == nullptr or not netinfo->connected) {
    SetHardTimer(App::refresh_timer_name, &taranis::App::refresh_data_maybe,
                 App::refresh_period);
    return;
  }
  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data, 0);
}
