#include "locationbox.h"

#include "events.h"

std::array<char, 256> input_text;

void taranis::keyboard_handler(char *text) {
  const auto event_handler = GetEventHandler();
  SendEventEx(event_handler, EVT_CUSTOM, CustomEvent::search_location, 0,
              &input_text);
}

void taranis::LocationBox::edit_location() {
  const auto title = GetLangText("Enter location");
  std::memset(input_text.data(), '\0', input_text.size());

  const auto formatted_location_name = format_location(this->model->location);
  std::memcpy(input_text.data(), formatted_location_name.data(),
              formatted_location_name.size());
  OpenKeyboard(title, input_text.data(), input_text.size() - 1, KBD_NORMAL,
               keyboard_handler);
}
