#include "locationbox.h"

#include "events.h"

std::array<char, 256> location;

void taranis::keyboard_handler(char *text) {
  const auto event_handler = GetEventHandler();
  SendEventEx(event_handler, EVT_CUSTOM, CustomEvent::new_location_requested, 0,
              &location);
}

void taranis::LocationBox::edit_location() {
  const auto title = T("Enter location");
  std::memset(location.data(), '\0', location.size());

  const auto current_location =
      this->model->location.town + ", " + this->model->location.country;
  std::memcpy(location.data(), current_location.data(),
              current_location.size());
  OpenKeyboard(title, location.data(), location.size() - 1, KBD_NORMAL,
               keyboard_handler);
}
