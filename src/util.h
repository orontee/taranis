#pragma once

#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>

#include "model.h"

namespace taranis {
std::vector<double>
normalize_temperatures(const std::vector<Condition> &conditions,
                       const int amplitude);

std::vector<double> normalize_rain(const std::vector<Condition> &conditions,
                                   const int lower_bound,
                                   const int upper_bound);

static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char ch) {
            return not std::isspace(ch) and ch != ',';
          }));
}

static inline void rtrim(std::string &s) {
  s.erase(
      std::find_if(s.rbegin(), s.rend(),
                   [](char ch) { return not std::isspace(ch) and ch != ','; })
          .base(),
      s.end());
}
} // namespace taranis
