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

std::vector<std::string> taranis::identify_moon_phases(const std::vector<DailyCondition> &conditions) {
  static constexpr std::array<long double, 5> phases{0.0, 0.25, 0.5, 0.75, 1.0};

  std::array<long double, phases.size()> min_distances{NAN, NAN, NAN, NAN, NAN};

  std::array<long double, phases.size()> phase_distances{NAN, NAN, NAN, NAN, NAN};
  std::array<std::vector<DailyCondition>::const_iterator, phases.size()> identified_phases{conditions.end(), conditions.end(), conditions.end(), conditions.end()};
  for (auto condition_it = conditions.begin(); condition_it < conditions.end(); ++condition_it) {
    const auto &condition = *condition_it;
    std::transform(phases.begin(), phases.end(), phase_distances.begin(), [&condition](long double phase) {
      return std::abs(condition.moon_phase - phase);
    });

    const auto min_distance = std::min_element(phase_distances.begin(), phase_distances.end());
    if (min_distance == phase_distances.end() or *min_distance >= 0.2) {
      continue;
    }
    const auto min_distance_offset = std::distance(phase_distances.begin(), min_distance);
    const auto current_min_distance = min_distances.begin() + min_distance_offset;
    const auto previously_identified_phase = identified_phases.begin() + min_distance_offset;
    if (*previously_identified_phase != conditions.end()) {
      if (*min_distance >= *current_min_distance) {
	continue;
      }
    }
    *current_min_distance = *min_distance;
    *previously_identified_phase = condition_it;
  }
  std::vector<std::string> phase_names;
  phase_names.resize(conditions.size());
  for (size_t identified_phase_index = 0; identified_phase_index < identified_phases.size(); ++identified_phase_index) {
    const auto &condition_it = identified_phases[identified_phase_index];
    if (condition_it == conditions.end()) {
      continue;
    }
    const auto name_to_update = phase_names.begin() + std::distance(conditions.begin(), condition_it);
    if (identified_phase_index == 0 or identified_phase_index == 4) {
      *(name_to_update) = "moon_new";
    } else if (identified_phase_index == 1) {
      *(name_to_update) = "first_quarter";
     } else if (identified_phase_index == 2) {
      *(name_to_update) = "full";
    } else if (identified_phase_index == 3) {
      *(name_to_update) = "last_quarter";
    }
  }
  return phase_names;
}
