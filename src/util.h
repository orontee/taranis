#pragma once

#include <vector>

#include "model.h"

namespace taranis {
  std::vector<double> normalize_temperatures(std::vector<Condition> conditions, int amplitude);
}
