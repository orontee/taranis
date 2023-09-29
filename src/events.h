#pragma once

enum CustomEvent {
  // Commands
  change_daily_forecast_display,
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

// Local variables:
// mode: c++-ts;
// End:
