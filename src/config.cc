#include "config.h"

#include <boost/log/trivial.hpp>
#include <cstring>
#include <inkview.h>

#include "events.h"
#include "icons.h"

#define GetLangText(str) str

namespace {
taranis::Config *that{nullptr};
}

namespace taranis {

static iconfig *config = nullptr;

char *unit_system_values[] = {const_cast<char *>(GetLangText("Standard")),
                              const_cast<char *>(GetLangText("Metric")),
                              const_cast<char *>(GetLangText("Imperial")),
                              nullptr};
// ☠️ index must match UnitSystem enum values…

char *start_with_daily_forecast_values[] = {
    const_cast<char *>(GetLangText("Hourly forecast")),
    const_cast<char *>(GetLangText("Daily forecast")), nullptr};

char *data_update_strategy_values[] = {
    const_cast<char *>(GetLangText("At startup")),
    const_cast<char *>(GetLangText("When obsolete")),
    const_cast<char *>(GetLangText("Manual")), nullptr};

char *custom_api_key_value[] = {const_cast<char *>(""), nullptr};

char *no_yes_values[] = {const_cast<char *>("@No"), const_cast<char *>("@Yes"),
                         nullptr};

// Note that translating is handled by the configuration editor but
// translations must be provided as usual

static iconfigedit software_config_template[] = {
    {CFG_ACTIONS, &icon_software_version,
     const_cast<char *>(GetLangText("Software version")), nullptr,
     const_cast<char *>("display_software_version"), nullptr, nullptr, nullptr},
    {CFG_ACTIONS, &icon_software_version_check,
     const_cast<char *>(GetLangText("Check newer version")), nullptr,
     const_cast<char *>("check_software_version"), nullptr, nullptr, nullptr},
    {0}};

static iconfigedit main_config_template[] = {
    {CFG_INDEX, &icon_measuring_tape, const_cast<char *>(GetLangText("Units")),
     nullptr, const_cast<char *>("unit_system"), const_cast<char *>("0"),
     unit_system_values, nullptr},
    {CFG_INDEX, &icon_display_settings,
     const_cast<char *>(GetLangText("Start screen")), nullptr,
     const_cast<char *>("start_with_daily_forecast"), const_cast<char *>("0"),
     start_with_daily_forecast_values, nullptr},
    {CFG_INDEX, &icon_refresh,
     const_cast<char *>(GetLangText("Data update strategy")), nullptr,
     const_cast<char *>("data_update_strategy"), const_cast<char *>("0"),
     data_update_strategy_values, nullptr},
    {CFG_TEXT, &icon_key, const_cast<char *>(GetLangText("Custom API key")),
     nullptr, const_cast<char *>("api_key"), const_cast<char *>(""),
     custom_api_key_value, nullptr},
    {CFG_INDEX, &icon_wallpaper,
     const_cast<char *>(GetLangText("Generate shutdown logo")), nullptr,
     const_cast<char *>("generate_shutdown_logo"), const_cast<char *>("0"),
     no_yes_values, nullptr},
    {CFG_SUBMENU, &icon_software_version,
     const_cast<char *>(GetLangText("Software")), nullptr, nullptr, nullptr,
     nullptr, software_config_template},
    {0}};

// Don't use hints since failed to find a way to change their font
// size...

Config::Config() {
  if (not config) {
    config =
        OpenConfig(Config::get_config_path().c_str(), main_config_template);
  }
}

bool Config::read_bool(const std::string &name, bool default_value) {
  return std::strcmp(
             ReadString(config, name.c_str(), default_value ? "1" : "0"),
             "1") == 0;
}

void Config::write_bool(const std::string &name, bool value) {
  WriteString(config, name.c_str(), value ? "1" : "0");
}

int Config::read_int(const std::string &name, int default_value) {
  return ReadInt(config, name.c_str(), default_value);
}

void Config::write_int(const std::string &name, int value) {
  WriteInt(config, name.c_str(), value);
}

std::string Config::read_string(const std::string &name,
                                const std::string &default_value) {
  return ReadString(config, name.c_str(), default_value.c_str());
}

void Config::write_string(const std::string &name, const std::string &value) {
  WriteString(config, name.c_str(), value.c_str());
}

std::string Config::read_secret(const std::string &name,
                                const std::string &default_value) {
  return ReadSecret(config, name.c_str(), default_value.c_str());
}

void Config::write_secret(const std::string &name, const std::string &value) {
  return WriteSecret(config, name.c_str(), value.c_str());
}

void Config::open_editor() {
  BOOST_LOG_TRIVIAL(debug) << "Opening config editor";

  this->changed = false;
  this->application_event_handler = GetEventHandler();
  that = this;

  OpenConfigEditor(GetLangText("Parameters"), config, main_config_template,
                   Config::config_changed, Config::item_changed);
}

std::string Config::get_config_path() {
  iprofile profile = CreateProfileStruct();
  const auto failed = GetCurrentProfileEx(&profile);
  const auto stem = not failed ? std::string{profile.path} + "/config"
                               : std::string{CONFIGPATH};
  const auto config_path = stem + "/taranis.cfg";

  BOOST_LOG_TRIVIAL(info) << "Config path " << config_path;

  return config_path;
}

void Config::item_changed(char *name) {
  if (that == nullptr) {
    BOOST_LOG_TRIVIAL(warning) << "Skipping item changed notification received "
                                  "while uninitialized config "
                               << name;
    return;
  }
  BOOST_LOG_TRIVIAL(debug) << "Change in config item " << name;

  if (std::strcmp(name, "+display_software_version") == 0) {
    BOOST_LOG_TRIVIAL(debug) << "Opening about dialog";
    SendEvent(that->application_event_handler, EVT_CUSTOM,
              CustomEvent::open_about_dialog, 0);
  } else if (std::strcmp(name, "+check_software_version") == 0) {
    SendEvent(that->application_event_handler, EVT_CUSTOM,
              CustomEvent::check_application_version, 0);
  } else {
    that->changed = true;
  }
}

void Config::config_changed() {
  if (that == nullptr) {
    BOOST_LOG_TRIVIAL(warning)
        << "Skipping config changed notification received "
        << "while uninitialized config";
    return;
  }
  if (not that->changed) {
    BOOST_LOG_TRIVIAL(debug) << "Config unchanged, nothing to save";
    return;
  }
  BOOST_LOG_TRIVIAL(debug) << "Saving new config";

  const auto saved = SaveConfig(config);
  if (saved) {
    BOOST_LOG_TRIVIAL(debug) << "Notifying that config changed";
    NotifyConfigChanged();
  } else {
    BOOST_LOG_TRIVIAL(debug) << "Failed to save config";
  }
}
} // namespace taranis
