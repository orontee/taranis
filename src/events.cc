#include "events.h"

std::string taranis::format_custom_event_param(int param) {
  if (param == CustomEvent::change_daily_forecast_display) {
    return "change_daily_forecast_display";
  }
  if (param == CustomEvent::search_location) {
    return "search_location";
  }
  if (param == CustomEvent::edit_location) {
    return "edit_location";
  }
  if (param == CustomEvent::open_alert_viewer) {
    return "open_alert_viewer";
  }
  if (param == CustomEvent::open_config_editor) {
    return "open_config_editor";
  }
  if (param == CustomEvent::refresh_data) {
    return "refresh_data";
  }
  if (param == CustomEvent::select_location_from_history) {
    return "select_location_from_history";
  }
  if (param == CustomEvent::select_location_from_list) {
    return "select_location_from_list";
  }
  if (param == CustomEvent::toggle_current_location_favorite) {
    return "toggle_current_location_favorite";
  }

  if (param == CustomEvent::model_daily_forecast_display_changed) {
    return "model_daily_forecast_display_changed";
  }
  if (param == CustomEvent::model_updated) {
    return "model_updated";
  }
  if (param == CustomEvent::warning_emitted) {
    return "warning_emitted";
  }
  return "Unknow custom event";
}

std::string taranis::format_event_type(int event_type) {
  // Note that char *iv_evttype(int event_type) is not complete...

  if (event_type == EVT_INIT) {
    return "EVT_INIT";
  }
  if (event_type == EVT_EXIT) {
    return "EVT_EXIT";
  }
  if (event_type == EVT_SHOW) {
    return "EVT_SHOW";
  }
  if (event_type == EVT_HIDE) {
    return "EVT_HIDE";
  }
  if (event_type == EVT_KEYDOWN) {
    return "EVT_KEYDOWN";
  }
  if (event_type == EVT_KEYPRESS) {
    return "EVT_KEYPRESS";
  }
  if (event_type == EVT_KEYUP) {
    return "EVT_KEYUP";
  }
  if (event_type == EVT_KEYRELEASE) {
    return "EVT_KEYRELEASE";
  }
  if (event_type == EVT_KEYREPEAT) {
    return "EVT_KEYREPEAT";
  }
  if (event_type == EVT_KEYPRESS_EXT) {
    return "EVT_KEYPRESS_EXT";
  }
  if (event_type == EVT_KEYRELEASE_EXT) {
    return "EVT_KEYRELEASE_EXT";
  }
  if (event_type == EVT_KEYREPEAT_EXT) {
    return "EVT_KEYREPEAT_EXT";
  }
  if (event_type == EVT_POINTERUP) {
    return "EVT_POINTERUP";
  }
  if (event_type == EVT_POINTERDOWN) {
    return "EVT_POINTERDOWN";
  }
  if (event_type == EVT_POINTERMOVE) {
    return "EVT_POINTERMOVE";
  }
  if (event_type == EVT_SCROLL) {
    return "EVT_SCROLL";
  }
  if (event_type == EVT_POINTERLONG) {
    return "EVT_POINTERLONG";
  }
  if (event_type == EVT_POINTERHOLD) {
    return "EVT_POINTERHOLD";
  }
  if (event_type == EVT_POINTERDRAG) {
    return "EVT_POINTERDRAG";
  }
  if (event_type == EVT_POINTERCANCEL) {
    return "EVT_POINTERCANCEL";
  }
  if (event_type == EVT_POINTERCHANGED) {
    return "EVT_POINTERCHANGED";
  }
  if (event_type == EVT_ORIENTATION) {
    return "EVT_ORIENTATION";
  }
  if (event_type == EVT_FOCUS) {
    return "EVT_FOCUS";
  }
  if (event_type == EVT_UNFOCUS) {
    return "EVT_UNFOCUS";
  }
  if (event_type == EVT_ACTIVATE) {
    return "EVT_ACTIVATE";
  }
  if (event_type == EVT_MTSYNC) {
    return "EVT_MTSYNC";
  }
  if (event_type == EVT_TOUCHUP) {
    return "EVT_TOUCHUP";
  }
  if (event_type == EVT_TOUCHDOWN) {
    return "EVT_TOUCHDOWN";
  }
  if (event_type == EVT_TOUCHMOVE) {
    return "EVT_TOUCHMOVE";
  }
  if (event_type == EVT_REPAINT) {
    return "EVT_REPAINT";
  }
  if (event_type == EVT_QN_MOVE) {
    return "EVT_QN_MOVE";
  }
  if (event_type == EVT_QN_RELEASE) {
    return "EVT_QN_RELEASE";
  }
  if (event_type == EVT_QN_BORDER) {
    return "EVT_QN_BORDER";
  }
  if (event_type == EVT_SNAPSHOT) {
    return "EVT_SNAPSHOT";
  }
  if (event_type == EVT_FSINCOMING) {
    return "EVT_FSINCOMING";
  }
  if (event_type == EVT_FSCHANGED) {
    return "EVT_FSCHANGED";
  }
  if (event_type == EVT_MP_STATECHANGED) {
    return "EVT_MP_STATECHANGED";
  }
  if (event_type == EVT_MP_TRACKCHANGED) {
    return "EVT_MP_TRACKCHANGED";
  }
  if (event_type == EVT_PREVPAGE) {
    return "EVT_PREVPAGE";
  }
  if (event_type == EVT_NEXTPAGE) {
    return "EVT_NEXTPAGE";
  }
  if (event_type == EVT_OPENDIC) {
    return "EVT_OPENDIC";
  }
  if (event_type == EVT_CONTROL_PANEL_ABOUT_TO_OPEN) {
    return "EVT_CONTROL_PANEL_ABOUT_TO_OPEN";
  }
  if (event_type == EVT_UPDATE) {
    return "EVT_UPDATE";
  }
  if (event_type == EVT_PANEL_BLUETOOTH_A2DP) {
    return "EVT_PANEL_BLUETOOTH_A2DP";
  }
  if (event_type == EVT_TAB) {
    return "EVT_TAB";
  }
  if (event_type == EVT_PANEL) {
    return "EVT_PANEL";
  }
  if (event_type == EVT_PANEL_ICON) {
    return "EVT_PANEL_ICON";
  }
  if (event_type == EVT_PANEL_TEXT) {
    return "EVT_PANEL_TEXT";
  }
  if (event_type == EVT_PANEL_PROGRESS) {
    return "EVT_PANEL_PROGRESS";
  }
  if (event_type == EVT_PANEL_MPLAYER) {
    return "EVT_PANEL_MPLAYER";
  }
  if (event_type == EVT_PANEL_USBDRIVE) {
    return "EVT_PANEL_USBDRIVE";
  }
  if (event_type == EVT_PANEL_NETWORK) {
    return "EVT_PANEL_NETWORK";
  }
  if (event_type == EVT_PANEL_CLOCK) {
    return "EVT_PANEL_CLOCK";
  }
  if (event_type == EVT_PANEL_BLUETOOTH) {
    return "EVT_PANEL_BLUETOOTH";
  }
  if (event_type == EVT_PANEL_TASKLIST) {
    return "EVT_PANEL_TASKLIST";
  }
  if (event_type == EVT_PANEL_OBREEY_SYNC) {
    return "EVT_PANEL_OBREEY_SYNC";
  }
  if (event_type == EVT_PANEL_SETREADINGMODE) {
    return "EVT_PANEL_SETREADINGMODE";
  }
  if (event_type == EVT_PANEL_SETREADINGMODE_INVERT) {
    return "EVT_PANEL_SETREADINGMODE_INVERT";
  }
  if (event_type == EVT_PANEL_FRONT_LIGHT) {
    return "EVT_PANEL_FRONT_LIGHT";
  }
  if (event_type == EVT_GLOBALREQUEST) {
    return "EVT_GLOBALREQUEST";
  }
  if (event_type == EVT_GLOBALACTION) {
    return "EVT_GLOBALACTION";
  }
  if (event_type == EVT_FOREGROUND) {
    return "EVT_FOREGROUND";
  }
  if (event_type == EVT_BACKGROUND) {
    return "EVT_BACKGROUND";
  }
  if (event_type == EVT_SUBTASKCLOSE) {
    return "EVT_SUBTASKCLOSE";
  }
  if (event_type == EVT_CONFIGCHANGED) {
    return "EVT_CONFIGCHANGED";
  }
  if (event_type == EVT_SAVESTATE) {
    return "EVT_SAVESTATE";
  }
  if (event_type == EVT_OBREEY_CONFIG_CHANGED) {
    return "EVT_OBREEY_CONFIG_CHANGED";
  }
  if (event_type == EVT_SDIN) {
    return "EVT_SDIN";
  }
  if (event_type == EVT_SDOUT) {
    return "EVT_SDOUT";
  }
  if (event_type == EVT_USBSTORE_IN) {
    return "EVT_USBSTORE_IN	";
  }
  if (event_type == EVT_USBSTORE_OUT) {
    return "EVT_USBSTORE_OUT";
  }
  if (event_type == EVT_BT_RXCOMPLETE) {
    return "EVT_BT_RXCOMPLETE";
  }
  if (event_type == EVT_BT_TXCOMPLETE) {
    return "EVT_BT_TXCOMPLETE";
  }
  if (event_type == EVT_SYNTH_ENDED) {
    return "EVT_SYNTH_ENDED";
  }
  if (event_type == EVT_DIC_CLOSED) {
    return "EVT_DIC_CLOSED";
  }
  if (event_type == EVT_SHOW_KEYBOARD) {
    return "EVT_SHOW_KEYBOARD";
  }
  if (event_type == EVT_TEXTCLEAR) {
    return "EVT_TEXTCLEAR";
  }
  if (event_type == EVT_EXT_KB) {
    return "EVT_EXT_KB	";
  }
  if (event_type == EVT_LETTER) {
    return "EVT_LETTER";
  }
  if (event_type == EVT_CALLBACK) {
    return "EVT_CALLBACK";
  }
  if (event_type == EVT_SCANPROGRESS) {
    return "EVT_SCANPROGRESS";
  }
  if (event_type == EVT_STOPSCAN) {
    return "EVT_STOPSCAN";
  }
  if (event_type == EVT_STARTSCAN) {
    return "EVT_STARTSCAN";
  }
  if (event_type == EVT_SCANSTOPPED) {
    return "EVT_SCANSTOPPED";
  }
  if (event_type == EVT_POSTPONE_TIMED_POWEROFF) {
    return "EVT_POSTPONE_TIMED_POWEROFF";
  }
  if (event_type == EVT_FRAME_ACTIVATED) {
    return "EVT_FRAME_ACTIVATED";
  }
  if (event_type == EVT_FRAME_DEACTIVATED) {
    return "EVT_FRAME_DEACTIVATED";
  }
  if (event_type == EVT_READ_PROGRESS_CHANGED) {
    return "EVT_READ_PROGRESS_CHANGED";
  }
  if (event_type == EVT_DUMP_BITMAPS_DEBUG_INFO) {
    return "EVT_DUMP_BITMAPS_DEBUG_INFO";
  }
  if (event_type == EVT_NET_CONNECTED) {
    return "EVT_NET_CONNECTED";
  }
  if (event_type == EVT_NET_DISCONNECTED) {
    return "EVT_NET_DISCONNECTED";
  }
  if (event_type == EVT_NET_FOUND_NEW_FW) {
    return "EVT_NET_FOUND_NEW_FW";
  }
  if (event_type == EVT_SYNTH_POSITION) {
    return "EVT_SYNTH_POSITION";
  }
  if (event_type == EVT_ASYNC_TASK_FINISHED) {
    return "EVT_ASYNC_TASK_FINISHED";
  }
  if (event_type == EVT_STOP_PLAYING) {
    return "EVT_STOP_PLAYING";
  }
  if (event_type == EVT_AVRCP_COMMAND) {
    return "EVT_AVRCP_COMMAND";
  }
  if (event_type == EVT_AUDIO_CHANGED) {
    return "EVT_AUDIO_CHANGED";
  }
  if (event_type == EVT_PACKAGE_JOB_CHANGED) {
    return "EVT_PACKAGE_JOB_CHANGED";
  }
  if (event_type == EVT_CUSTOM) {
    return "EVT_CUSTOM";
  }
  return "Unknow event type";
}
