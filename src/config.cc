#include "config.h"
#include "inkview.h"

std::unique_ptr<iconfig, void (*)(iconfig *)> config{nullptr, &CloseConfig};

char editor_title[] = "Configuration";

iconfigedit config_template[] = {
    {0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}};

taranis::Config::Config() {
  if (not config) {
    config.reset(
        OpenConfig(Config::get_config_path().c_str(), config_template));
  }
}

int taranis::Config::read_int(const std::string &name,
                              int default_value) {
  return ReadInt(config.get(), name.c_str(), default_value);
}

void taranis::Config::write_int(const std::string &name, int value) {
  WriteInt(config.get(), name.c_str(), value);
}

std::string taranis::Config::read_string(const std::string &name,
                                         const std::string &default_value) {
  return ReadString(config.get(), name.c_str(), default_value.c_str());
}

void taranis::Config::write_string(const std::string &name,
                                   const std::string &value) {
  WriteString(config.get(), name.c_str(), value.c_str());
}

std::string taranis::Config::read_secret(const std::string &name,
                                         const std::string &default_value) {
  return ReadSecret(config.get(), name.c_str(), default_value.c_str());
}

void taranis::Config::write_secret(const std::string &name,
                                   const std::string &value) {
  return WriteSecret(config.get(), name.c_str(), value.c_str());
}

void taranis::Config::config_changed() {
  const auto saved = SaveConfig(config.get());
  if (saved) {
    NotifyConfigChanged();
  }
  config.reset(nullptr);
}
