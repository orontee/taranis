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
taranis::normalize_precipitations(const std::vector<Condition> &conditions,
                                  const int lower_bound,
                                  const int upper_bound) {
  assert(lower_bound < upper_bound);

  const double precipitations_threshold = 60;
  // The scale is computed to reach upper_bound for precipitations
  // >60mm
  //
  // Note that 305mm is the record of rain precipitation in one hour,
  // Holt, Missouri, United States, 22 June 1947!

  std::vector<double> precipitations;
  precipitations.reserve(conditions.size());
  for (const auto &condition : conditions) {
    precipitations.push_back(max_number(condition.rain, condition.snow));
  }

  double min = NAN;
  double max = precipitations_threshold;
  ;
  for (const auto &precipitation : precipitations) {
    if (std::isnan(precipitation)) {
      continue;
    }
    const auto value = std::min(precipitation, precipitations_threshold);
    if (std::isnan(min) or value < min) {
      min = value;
    }
  }
  if (std::isnan(min)) {
    return precipitations;
  } else if (min == max) {
    return std::vector<double>(conditions.size(), upper_bound);
  }

  std::vector<double> normalized;
  normalized.reserve(conditions.size());
  for (const auto &precipitation : precipitations) {
    if (std::isnan(precipitation)) {
      normalized.push_back(NAN);
      continue;
    }
    normalized.push_back((upper_bound - lower_bound) / (max - min) *
                             (precipitation - min) +
                         lower_bound);
  }
  return normalized;
}

double taranis::max_number(double value_one, double value_two) {
  if (not std::isnan(value_one) and not std::isnan(value_two)) {
    return std::max(value_one, value_two);
  } else if (std::isnan(value_two)) {
    return value_one;
  } else if (std::isnan(value_one)) {
    return value_two;
  }
  return NAN;
}

constexpr char time_format[] = "%H:%M";
constexpr char full_date_format[] = "%R (%F)";
static char formatted_time[100];

std::string taranis::format_time(const std::time_t &time, bool full) {
  auto format = full ? full_date_format : time_format;
  auto calendar_time = std::localtime(&time);
  if (calendar_time == nullptr) {
    return "";
  }
  std::strftime(formatted_time, sizeof(formatted_time), format, calendar_time);
  // TODO should use GetLangTime() to use user "locale" but don't know
  // how it works…
  return formatted_time;
}

const char *weekdays[7] = {"@Sun", "@Mon", "@Tue", "@Wed",
                           "@Thu", "@Fri", "@Sat"};

std::string taranis::format_day(const std::time_t &time) {
  auto calendar_time = std::localtime(&time);
  if (calendar_time == nullptr) {
    return "";
  }
  return GetLangText(weekdays[calendar_time->tm_wday]);
}
