#pragma once

enum CustomEvent {
  about_dialog_requested,
  change_unit_system,
  model_updated,
  new_location_requested,
  refresh_requested,
  warning_emitted,
};

enum CustomEventParam {
  invalid_location,
};
