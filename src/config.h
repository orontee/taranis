#pragma once

#include <inkview.h>
#include <memory>
#include <string>

using namespace std::string_literals;

namespace taranis {

  struct Config {
    Config()
      : config{OpenConfig("/mnt/ext1/system/config/taranis.cfg", nullptr),
      &CloseConfig} {}

    std::string read_string(const std::string &name,
                            const std::string &default_value) {
      return ReadString(this->config.get(), name.c_str(), default_value.c_str());
    }

    void write_string(const std::string &name, const std::string &value) {
      WriteString(this->config.get(), name.c_str(), value.c_str());
    }

    std::string read_secret(const std::string &name,
                            const std::string &default_value) {
      return ReadSecret(this->config.get(), name.c_str(), default_value.c_str());
    }

    void write_secret(const std::string &name, const std::string &value) {
      return WriteSecret(this->config.get(), name.c_str(), value.c_str());
    }

    bool save() { return SaveConfig(this->config.get()); }

  private:
    std::unique_ptr<iconfig, void(*)(iconfig*)> config;
  };

}
