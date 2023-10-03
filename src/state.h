#pragma once

#include <json/json.h>
#include <memory>
#include <string>

#include "model.h"

namespace taranis {
class ApplicationState {

public:
  ApplicationState(std::shared_ptr<Model> model) : model{model} {
    this->restore();
  }

  ~ApplicationState() { this->dump(); }

  void restore();

  void dump();

private:
  std::shared_ptr<Model> model;

  static std::string get_application_state_path();

  void restore_location_history(const Json::Value &root);

  void dump_location_history(Json::Value &root);
};
} // namespace taranis
