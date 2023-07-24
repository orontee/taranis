#pragma once

#include <algorithm>
#include <ctime>
#include <experimental/optional>
#include <string>
#include <vector>

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

struct Location {
  std::string town;
  std::string country;
};

enum Weather {
  unknown,
  cloudly,
  storm,
  sunny,
};

struct Forecast {
  std::time_t date{0};
  long double temperature;
  long double felt_temperature;
  Weather weather;
  int humidity;
  long double wind_speed;
};

struct Model {
  std::string source;
  std::time_t refresh_date{0};
  Location location;

  std::optional<Forecast> current_forecast;
  std::vector<Forecast> hourly_forecast;

  std::vector<int> normalized_hourly_temperatures(int amplitude) const {
    if (this->hourly_forecast.size() < 2) {
      return {};
    }
    const auto [min, max] = std::minmax_element(
        this->hourly_forecast.begin(), this->hourly_forecast.end(),
        [](const Forecast lhs, const Forecast rhs) {
          return lhs.temperature < rhs.temperature;
        });

    const auto min_temperature = min->temperature;
    const auto max_temperature = max->temperature;

    if (min_temperature == max_temperature) {
      return std::vector<int>(hourly_forecast.size(), amplitude / 2);
    }

    std::vector<int> normalized;
    normalized.reserve(hourly_forecast.size());

    const auto scale_factor = amplitude / (max_temperature - min_temperature);

    for (const auto &forecast : hourly_forecast) {
      normalized.push_back((forecast.temperature - min_temperature) *
                           scale_factor);
    }
    return normalized;
  }
};
