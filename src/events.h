#pragma once

enum CustomEvent {
  about_dialog_requested,
  config_editor_requested,
  model_updated,
  new_location_requested,
  refresh_requested,
  warning_emitted,
};

enum CustomEventParam {
  invalid_location,
};
