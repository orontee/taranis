#include "util.h"

#include <algorithm>

std::vector<double>
taranis::normalize_temperatures(std::vector<Condition> conditions,
                                int amplitude) {
  if (conditions.size() < 2) {
    return {};
  }
  const auto [min, max] =
      std::minmax_element(conditions.begin(), conditions.end(),
                          [](const Condition lhs, const Condition rhs) {
                            return lhs.temperature < rhs.temperature;
                          });

  const auto min_temperature = min->temperature;
  const auto max_temperature = max->temperature;

  if (min_temperature == max_temperature) {
    return std::vector<double>(conditions.size(), amplitude / 2.0);
  }

  std::vector<double> normalized;
  normalized.reserve(conditions.size());

  const auto scale_factor = amplitude / (max_temperature - min_temperature);

  for (const auto &condition : conditions) {
    normalized.push_back((condition.temperature - min_temperature) *
                         scale_factor);
  }
  return normalized;
}
