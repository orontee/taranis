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

inline void check_software_version(const std::string &version) {
  // expected form V632.6.7.1405
  std::stringstream to_parse{version};
  std::string token;
  if (std::getline(to_parse, token, '.')) {
    std::getline(to_parse, token, '.');
    try {
      if (std::stoi(token) >= 6) {
        return;
      }
    } catch (const std::invalid_argument &error) {
    }
  }
  throw UnsupportedSoftwareVersion{};
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

std::string format_time(const TimePoint &time, bool round = false);

std::string format_day(const TimePoint &time);

std::string format_short_date(const TimePoint &time);

std::string format_full_date(const TimePoint &time);

std::string format_duration(const TimePoint &start, const TimePoint &end);

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

std::pair<std::string, std::string>
parse_location_description(const std::string &description);
} // namespace taranis
