#include "alerts.h"

#include <iomanip>
#include <sstream>

#include "events.h"
#include "util.h"

#define OK_BUTTON_INDEX 0
#define NEXT_ALERT_BUTTON_INDEX 1

static size_t alert_index{0};
static size_t alert_count{0};

inline bool is_next_alert_button_at(int button_index) {
  return (alert_index + 1 < alert_count) ? button_index == 1 : false;
}

void taranis::alert_dialog_handler(int button_index) {
  if (is_next_alert_button_at(button_index)) {
    const auto event_handler = GetEventHandler();
    ++alert_index;
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::display_alert, 0);
  } else {
    alert_index = 0;
  }
  CloseDialog();
}

void taranis::AlertsButton::open_dialog_maybe() {
  alert_count = this->model->alerts.size();

  if (alert_count == 0) {
    return;
  }
  if (alert_index >= alert_count) {
    return;
  }

  const auto &alert = this->model->alerts.at(alert_index);

  const auto dialog_title =
      not alert.event.empty() ? alert.event : GetLangText("Alert");
  // 😕 Looks like dialog title isn't displayed at all!

  std::stringstream alert_text;
  if (not alert.event.empty()) {
    alert_text << alert.event << std::endl << std::endl;
  }
  alert_text << alert.description << std::endl << std::endl;
  if (alert.start_date) {
    alert_text << GetLangText("Start") << " "
               << format_time(alert.start_date, true) << std::endl;
    if (alert.end_date) {
      alert_text << GetLangText("Duration") << " "
                 << format_duration(alert.start_date, alert.end_date)
                 << std::endl;
    }
  }
  if (not alert.sender.empty()) {
    alert_text << GetLangText("Source") << " " << alert.sender;
  }

  const auto first_button_text = alert_index + 1 < alert_count
                                     ? GetLangText("Next alert")
                                     : GetLangText("Ok");
  const auto second_button_text =
      alert_index + 1 < alert_count ? GetLangText("Ok") : nullptr;

  Dialog(ICON_WARNING, dialog_title.c_str(), alert_text.str().c_str(),
         first_button_text, second_button_text, &alert_dialog_handler);
}
