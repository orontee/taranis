#include "util.h"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <inkview.h>

using namespace std::chrono_literals;

std::vector<double>
taranis::normalize_temperatures(const std::vector<Condition> &conditions,
                                const int amplitude) {
  BOOST_LOG_TRIVIAL(debug) << "Normalizing temperatures";

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
  BOOST_LOG_TRIVIAL(debug) << "Normalizing precipitations";

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
static char formatted_time[100];

std::string taranis::format_time(const TimePoint &time, bool round) {
  const time_t time_since_epoch{static_cast<long>(
      std::chrono::duration_cast<std::chrono::seconds>(time - TimePoint{})
          .count())};
  const auto calendar_time = std::localtime(&time_since_epoch);
  if (calendar_time == nullptr) {
    return "";
  }
  if (round and calendar_time->tm_min != 0) {
    if (calendar_time->tm_min > 30) {
      calendar_time->tm_hour += 1;
      calendar_time->tm_min = 0;
      calendar_time->tm_sec = 0;
    } else {
      calendar_time->tm_min = 0;
      calendar_time->tm_sec = 0;
    }
  }
  std::strftime(formatted_time, sizeof(formatted_time), time_format,
                calendar_time);
  // Should use GetLangTime() to use user "locale" but don't know how
  // it works…
  return formatted_time;
}

const char *weekdays[7] = {"@Sun", "@Mon", "@Tue", "@Wed",
                           "@Thu", "@Fri", "@Sat"};

std::string taranis::format_day(const TimePoint &time) {
  const time_t time_since_epoch{static_cast<long>(
      std::chrono::duration_cast<std::chrono::seconds>(time - TimePoint{})
          .count())};
  const auto calendar_time = std::localtime(&time_since_epoch);
  if (calendar_time == nullptr) {
    return "";
  }
  return GetLangText(weekdays[calendar_time->tm_wday]);
}

const char *months[12] = {"@Jan", "@Feb", "@Mar", "@Apr", "@May", "@Jun",
                          "@Jul", "@Aug", "@Sep", "@Oct", "@Nov", "@Dec"};

std::string taranis::format_short_date(const TimePoint &time) {
  const time_t time_since_epoch{static_cast<long>(
      std::chrono::duration_cast<std::chrono::seconds>(time - TimePoint{})
          .count())};
  const auto calendar_time = std::localtime(&time_since_epoch);
  if (calendar_time == nullptr) {
    return "";
  }
  return std::to_string(calendar_time->tm_mday) + " " +
         GetLangText(months[calendar_time->tm_mon]);
}

std::string taranis::format_date(const TimePoint &time, bool shortcut) {
  const time_t time_since_epoch{static_cast<long>(
      std::chrono::duration_cast<std::chrono::seconds>(time - TimePoint{})
          .count())};
  if (shortcut) {
    auto calendar_time = std::localtime(&time_since_epoch);
    if (calendar_time) {
      const auto time_year = calendar_time->tm_year;
      const auto time_year_day = calendar_time->tm_yday;
      const auto time_month = calendar_time->tm_mon;
      const auto time_month_day = calendar_time->tm_mday;

      const time_t now_since_epoch{
          static_cast<long>(std::chrono::duration_cast<std::chrono::seconds>(
                                std::chrono::system_clock::now() - TimePoint{})
                                .count())};
      calendar_time = std::localtime(&now_since_epoch);
      if (calendar_time) {
        const auto current_year = calendar_time->tm_year;
        const auto current_year_day = calendar_time->tm_yday;
        const auto current_month = calendar_time->tm_mon;
        const auto current_month_day = calendar_time->tm_mday;

        if (current_year == time_year and current_year_day == time_year_day) {
          return GetLangText("Today");
        } else if ((current_year == time_year and
                    current_year_day == time_year_day + 1) or
                   ((current_year == time_year + 1 and current_year_day == 0 and
                     time_month_day == 31 and time_month == 11))) {
          return GetLangText("Yesterday");
        } else if ((current_year == time_year and
                    current_year_day + 1 == time_year_day) or
                   ((current_year + 1 == time_year and time_year_day == 0 and
                     current_month_day == 31 and current_month == 11))) {
          return GetLangText("Tomorrow");
        }
      }
    }
  }
  return std::string{DateStr(time_since_epoch)};
}

std::string taranis::format_full_date(const TimePoint &time) {
  time_t time_since_epoch{static_cast<long>(
      std::chrono::duration_cast<std::chrono::seconds>(time - TimePoint{})
          .count())};
  auto calendar_time = std::localtime(&time_since_epoch);
  if (calendar_time == nullptr) {
    return "";
  }
  std::strftime(formatted_time, sizeof(formatted_time), time_format,
                calendar_time);
  // TODO should use GetLangTime() to use user "locale" but don't know
  // how it works…

  return std::string{DateStr(time_since_epoch)} + ", " +
         std::string{formatted_time};
}

std::string taranis::format_duration(const TimePoint &start,
                                     const TimePoint &end) {
  const auto duration =
      std::chrono::duration_cast<std::chrono::hours>((end - start));
  if (duration < 1h) {
    return "<1h";
  }
  std::stringstream duration_text;
  duration_text << static_cast<int>(
                       std::chrono::duration_cast<std::chrono::hours>(
                           (end - start))
                           .count())
                << "h";
  return duration_text.str();
}

int taranis::remaining_hours(const TimePoint &time) {
  time_t time_since_epoch{static_cast<long>(
      std::chrono::duration_cast<std::chrono::seconds>(time - TimePoint{})
          .count())};
  auto calendar_time = std::localtime(&time_since_epoch);
  if (calendar_time == nullptr) {
    return 0;
  }
  return 24 - calendar_time->tm_hour;
}

int taranis::passed_hours(const TimePoint &time) {
  time_t time_since_epoch{static_cast<long>(
      std::chrono::duration_cast<std::chrono::seconds>(time - TimePoint{})
          .count())};
  auto calendar_time = std::localtime(&time_since_epoch);
  if (calendar_time == nullptr) {
    return 0;
  }
  return calendar_time->tm_hour;
}

std::pair<std::string, std::string>
taranis::parse_location_description(const std::string &description) {
  BOOST_LOG_TRIVIAL(debug) << "Parsing location description";

  std::stringstream to_parse{description};
  std::vector<std::string> tokens;
  std::string token;
  while (std::getline(to_parse, token, ',')) {
    ltrim(token);
    rtrim(token);
    tokens.push_back(token);
  }
  if (tokens.size() == 1) {
    auto town = tokens[0];
    town[0] = std::toupper(town[0]);
    return {town, ""};
  } else if (tokens.size() > 1) {
    auto country = tokens[tokens.size() - 1];
    country[0] = std::toupper(country[0]);

    auto town = description.substr(0, description.size() - country.size());
    ltrim(town);
    rtrim(town);
    town[0] = std::toupper(town[0]);

    return {town, country};
  }
  BOOST_LOG_TRIVIAL(error) << "Failed to parse location description "
                           << description;
  throw InvalidLocation{};
}
