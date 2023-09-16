#include "locationbox.h"

#include "events.h"
#include "inkview.h"
#include "util.h"

std::array<char, 256> location;

void taranis::keyboard_handler(char *text) {
  const auto event_handler = GetEventHandler();
  SendEventEx(event_handler, EVT_CUSTOM, CustomEvent::change_location, 0,
              &location);
}

void taranis::LocationBox::edit_location() {
  const auto title = T("Enter location");
  std::memset(location.data(), '\0', location.size());

  const auto current_location = format_location(this->model->location);
  std::memcpy(location.data(), current_location.data(),
              current_location.size());
  OpenKeyboard(title, location.data(), location.size() - 1, KBD_NORMAL,
               keyboard_handler);
}
