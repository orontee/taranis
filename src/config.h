#pragma once

#include <inkview.h>
#include <memory>
#include <string>

namespace taranis {

struct Config {
  Config();

  int read_int(const std::string &name, int default_value);

  void write_int(const std::string &name, int value);

  std::string read_string(const std::string &name,
                          const std::string &default_value);

  void write_string(const std::string &name, const std::string &value);

  std::string read_secret(const std::string &name,
                          const std::string &default_value);

  void write_secret(const std::string &name, const std::string &value);

  static std::string get_config_path() {
    iprofile profile = CreateProfileStruct();
    const auto failed = GetCurrentProfileEx(&profile);
    if (not failed) {
      return std::string{profile.path} + "/config/taranis.cfg";
    }
    return std::string{CONFIGPATH} + "/taranis.cfg";
  }

  static void config_changed();
};

} // namespace taranis
