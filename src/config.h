#pragma once

#include <boost/log/trivial.hpp>
#include <inkview.h>
#include <memory>
#include <string>

namespace taranis {

struct Config {
  Config();

  bool read_bool(const std::string &name, bool default_value);

  void write_bool(const std::string &name, bool value);

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
    const auto stem = not failed ? std::string{profile.path} + "/config"
                                 : std::string{CONFIGPATH};
    const auto config_path = stem + "/taranis.cfg";

    BOOST_LOG_TRIVIAL(info) << "Config path " << config_path;

    return config_path;
  }

  static void config_changed();
};

} // namespace taranis
