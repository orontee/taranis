#pragma once

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include "errors.h"
#include "model.h"

namespace taranis {
class Units {
public:
  Units(std::shared_ptr<Model> model) : model{model} {}

  std::string format_temperature(int temperature,
                                 bool short_unit = true) const {
    if (short_unit) {
      return std::to_string(temperature) + "°";
    }
    if (this->model->unit_system == UnitSystem::standard) {
      return std::to_string(temperature) + "K";
    } else if (this->model->unit_system == UnitSystem::metric) {
      return std::to_string(temperature) + "℃";
    } else if (this->model->unit_system == UnitSystem::imperial) {
      return std::to_string(temperature) + "℉";
    }
    throw UnsupportedUnitSystem{};
  }

  std::string format_distance(int distance) const {
    return std::to_string(distance) + "m";
  }

  std::string format_precipitation(long double precipitation,
                                   bool one_hour = true) const {
    if (one_hour) {
      if (precipitation < 1) {
        return "<1mm";
      }
      std::stringstream text;
      text << std::setprecision(1) << precipitation << "mm";
      return text.str();
    }
    return std::to_string(precipitation) + "mm/h";
  }

  std::string format_pressure(int pressure) const {
    return std::to_string(pressure) + "hPa";
  }

  std::string format_speed(long double speed) const {
    if (this->model->unit_system == UnitSystem::standard ||
        this->model->unit_system == UnitSystem::metric) {
      return std::to_string(static_cast<int>(speed)) + "m/s";
    } else if (this->model->unit_system == UnitSystem::imperial) {
      return std::to_string(static_cast<int>(speed)) + "mi/h";
    }
    throw UnsupportedUnitSystem{};
  }

  std::string to_string() const {
    if (this->model->unit_system == UnitSystem::standard) {
      return "standard";
    } else if (this->model->unit_system == UnitSystem::metric) {
      return "metric";
    } else if (this->model->unit_system == UnitSystem::imperial) {
      return "imperial";
    }
    throw UnsupportedUnitSystem{};
  }

private:
  std::shared_ptr<Model> model;
};
} // namespace taranis
