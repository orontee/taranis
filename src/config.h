#pragma once

#include <inkview.h>
#include <memory>
#include <sstream>
#include <string>

using namespace std::string_literals;

namespace taranis {

struct Config {
  Config() : internal{nullptr}, config{nullptr, &CloseConfig} {}

  std::string read_string(const std::string &name,
                          const std::string &default_value) {
    this->ensure_config_initialized();
    return ReadString(this->config.get(), name.c_str(), default_value.c_str());
  }

  void write_string(const std::string &name, const std::string &value) {
    this->ensure_config_initialized();
    WriteString(this->config.get(), name.c_str(), value.c_str());
  }

  std::string read_secret(const std::string &name,
                          const std::string &default_value) {
    this->ensure_config_initialized();
    return ReadSecret(this->config.get(), name.c_str(), default_value.c_str());
  }

  void write_secret(const std::string &name, const std::string &value) {
    this->ensure_config_initialized();
    return WriteSecret(this->config.get(), name.c_str(), value.c_str());
  }

  bool save() { return SaveConfig(this->config.get()); }

  void reset() { this->internal = nullptr; }

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

private:
  iconfigedit *internal;
  std::unique_ptr<iconfig, void (*)(iconfig *)> config;

  void ensure_config_initialized() {
    const auto config_path = Config::get_config_path();

    if (not this->internal) {
      this->config.reset(OpenConfig(config_path.c_str(), this->internal));
    }
  }
};

} // namespace taranis
