#pragma once

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

  void open_editor();

  static std::string get_config_path();

  static void config_changed();
};

} // namespace taranis
