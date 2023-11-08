#pragma once

#include <inkview.h>
#include <string>

namespace taranis {
enum CustomEvent {
  // Commands
  change_daily_forecast_display,
  search_location,
  change_location,
  open_alert_viewer,
  open_config_editor,
  refresh_data,
  select_location_from_history,
  select_location_from_list,
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

enum CallContext {
  triggered_by_application_startup,
  triggered_by_configuration_change,
  triggered_by_model_change,
  triggered_by_timer,
  triggered_by_user,
};

std::string format_custom_event_param(int param);

std::string format_event_type(int event_type);
} // namespace taranis

// Local variables:
// mode: c++-ts;
// End:
