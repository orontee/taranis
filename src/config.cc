#include "config.h"

#include <boost/log/trivial.hpp>
#include <cstring>
#include <inkview.h>

#include "icons.h"

#define GetLangText(str) str

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

char *custom_api_key_value[] = {const_cast<char *>(""), nullptr};

char *no_yes_values[] = {const_cast<char *>("@No"), const_cast<char *>("@Yes"),
                         nullptr};

// Note that translating is handled by the configuration editor but
// translations must be provided as usual

static iconfigedit config_template[] = {
    {CFG_INDEX, &icon_measuring_tape, const_cast<char *>(GetLangText("Units")),
     nullptr, const_cast<char *>("unit_system"), const_cast<char *>("0"),
     unit_system_values, nullptr},
    {CFG_INDEX, &icon_display_settings,
     const_cast<char *>(GetLangText("Start screen")), nullptr,
     const_cast<char *>("start_with_daily_forecast"), const_cast<char *>("0"),
     start_with_daily_forecast_values, nullptr},
    {CFG_TEXT, &icon_key, const_cast<char *>(GetLangText("Custom API key")),
     nullptr, const_cast<char *>("api_key"), const_cast<char *>(""),
     custom_api_key_value, nullptr},
    {CFG_INDEX, &icon_wallpaper,
     const_cast<char *>(GetLangText("Generate shutdown logo")), nullptr,
     const_cast<char *>("generate_shutdown_logo"), const_cast<char *>("0"),
     no_yes_values, nullptr},
    {0}};

// Don't use hints since failed to find a way to change their font
// size...

Config::Config() {
  if (not config) {
    config = OpenConfig(Config::get_config_path().c_str(), config_template);
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

  OpenConfigEditor(GetLangText("Parameters"), config, config_template,
                   Config::config_changed, nullptr);
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

void Config::config_changed() {
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
