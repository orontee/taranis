#pragma once

#include <inkview.h>
#include <string>

namespace taranis {
enum CustomEvent {
  // Commands
  change_daily_forecast_display,
  search_location,
  change_location,
  change_unit_system,
  display_alert,
  refresh_data,
  select_location_from_history,
  show_about_dialog,
  toggle_current_location_favorite,

  // Events
  model_daily_forecast_display_changed,
  model_updated,
  warning_emitted,
};

enum CustomEventParam {
  invalid_location,
};

std::string format_custom_event_param(int param);

std::string format_event_type(int event_type);
} // namespace taranis

// Local variables:
// mode: c++-ts;
// End:
