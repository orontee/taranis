#pragma once

#include <algorithm>
#include <cctype>
#include <ctime>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "errors.h"
#include "model.h"

namespace taranis {
std::vector<double>
normalize_temperatures(const std::vector<Condition> &conditions,
                       const int amplitude);

std::vector<double>
normalize_precipitations(const std::vector<Condition> &conditions,
                         const int lower_bound, const int upper_bound);

double max_number(double value_one, double value_two);

inline void firmware_version_greater_than(const std::string &version,
                                          int major_version,
                                          int minor_version = 0) {
  // expected form V632.6.7.1405
  std::stringstream to_parse{version};
  std::string token;
  if (std::getline(to_parse, token, '.')) {
    std::getline(to_parse, token, '.');
    try {
      if (std::stoi(token) >= major_version) {
        if (minor_version == 0) {
          return;
        }
        std::getline(to_parse, token, '.');
        if (std::stoi(token) >= minor_version) {
          return;
        }
      }
    } catch (const std::invalid_argument &error) {
    }
  }
  throw UnsupportedFirmwareVersion{};
}

inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch) {
            return not std::isspace(ch) and ch != ',';
          }));
}

inline void rtrim(std::string &s) {
  s.erase(
      std::find_if(s.rbegin(), s.rend(),
                   [](char ch) { return not std::isspace(ch) and ch != ','; })
          .base(),
      s.end());
}

TimePoint from_iso8601(const std::string &timestamp_string);

std::string now_as_iso8601_with_hour(int delta_hours = 0);

std::string now_as_iso8601(int delta_days = 0);

// 17:05
std::string format_time(const TimePoint &time, bool round = false);

// Sat
std::string format_short_day(const TimePoint &time);

// 21 Oct
std::string format_short_date(const TimePoint &time);

// Saturday, 21st October 2023, Today or Tomorrow
std::string format_date(const TimePoint &time, bool shortcut = false);

// Saturday, 21st October 2023, 17:05
std::string format_full_date(const TimePoint &time);

bool are_same_day(const TimePoint &first, const TimePoint &second);

int get_duration_minutes(const TimePoint &start, const TimePoint &end);

// <1h, 1h, 2h, etc.
std::string format_duration(const TimePoint &start, const TimePoint &end);

int remaining_hours(const TimePoint &time);

int passed_hours(const TimePoint &time);

std::string format_moon_phase(double moon_phase);

inline std::string format_location(const std::string &town,
                                   const std::string &country) {
  std::string text = town;
  if (not country.empty()) {
    text += ", " + country;
  }
  return text;
}

inline std::string format_location(const Location &location,
                                   bool with_state = false) {
  std::string text = location.name;
  if (not location.country.empty()) {
    text += ", " + location.country;
  }
  if (with_state and not location.state.empty()) {
    text += " (" + location.state + ")";
  }
  return text;
}

bool delay_exhausted_from(const TimePoint &time, int delay);

std::pair<std::string, std::string>
parse_location_description(const std::string &description);
} // namespace taranis
