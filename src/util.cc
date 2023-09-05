#include "util.h"

#include <algorithm>
#include <cassert>
#include <cmath>

std::vector<double>
taranis::normalize_temperatures(const std::vector<Condition> &conditions,
                                const int amplitude) {
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

std::vector<double>
taranis::normalize_rain(const std::vector<Condition> &conditions,
                        const int lower_bound, const int upper_bound) {
  assert(lower_bound < upper_bound);

  const long double rain_threshold = 60;
  // The scale is computed to reach upper_bound for precipitations
  // >60mm
  //
  // Note that 305mm is the record of rain precipitation in one hour,
  // Holt, Missouri, United States, 22 June 1947!

  double min = NAN;
  double max = rain_threshold;
  ;
  for (const auto &condition : conditions) {
    if (std::isnan(condition.rain)) {
      continue;
    }
    const auto value = std::min(condition.rain, rain_threshold);
    if (std::isnan(min) or value < min) {
      min = value;
    }
  }
  if (std::isnan(min)) {
    return std::vector<double>(conditions.size(), NAN);
  } else if (min == max) {
    return std::vector<double>(conditions.size(), upper_bound);
  }

  std::vector<double> normalized;
  normalized.reserve(conditions.size());
  for (const auto &condition : conditions) {
    if (std::isnan(condition.rain)) {
      normalized.push_back(NAN);
      continue;
    }
    normalized.push_back((upper_bound - lower_bound) / (max - min) *
                             (condition.rain - min) +
                         lower_bound);
  }
  return normalized;
}
