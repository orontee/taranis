#pragma once

enum CustomEvent {
  about_dialog_requested,
  change_daily_forecast_display,
  change_unit_system,
  display_alert,
  model_updated,
  new_location_requested,
  new_location_from_history,
  refresh_requested,
  warning_emitted,
};

enum CustomEventParam {
  invalid_location,
};
