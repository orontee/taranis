#pragma once

#include <inkview.h>
#include <memory>
#include <sstream>
#include <string>

using namespace std::string_literals;

namespace taranis {

struct Config {
  Config();

  std::string read_string(const std::string &name,
                          const std::string &default_value);

  void write_string(const std::string &name, const std::string &value);

  std::string read_secret(const std::string &name,
                          const std::string &default_value);

  void write_secret(const std::string &name, const std::string &value);

  static std::string get_config_path() {
    std::stringstream config_path;

    iprofile profile = CreateProfileStruct();
    const auto failed = GetCurrentProfileEx(&profile);
    if (not failed) {
      config_path << profile.path << "/config/taranis.cfg";
    } else {
      config_path << CONFIGPATH << "/taranis.cfg";
    }
    return config_path.str();
  }

  static void config_changed();
};

} // namespace taranis
