#pragma once

#include <algorithm>
#include <cmath>
#include <ctime>
#include <experimental/optional>
#include <list>
#include <string>
#include <vector>

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {

enum UnitSystem { standard = 0, metric = 1, imperial = 2 };

struct Location {
  std::string town;
  std::string country;

  bool operator==(const Location &other) const {
    return this->country == other.country and this->town == other.town;
  }
};

enum Weather {
  // Group 2xx: Thunderstorm
  THUNDERSTORM_WITH_LIGHT_RAIN = 200,
  THUNDERSTORM_WITH_RAIN = 201,
  THUNDERSTORM_WITH_HEAVY_RAIN = 202,
  LIGHT_THUNDERSTORM = 210,
  THUNDERSTORM = 211,
  HEAVY_THUNDERSTORM = 212,
  RAGGED_THUNDERSTORM = 221,
  THUNDERSTORM_WITH_LIGHT_DRIZZLE = 230,
  THUNDERSTORM_WITH_DRIZZLE = 231,
  THUNDERSTORM_WITH_HEAVY_DRIZZLE = 232,

  // Group 3xx: Drizzle
  LIGHT_INTENSITY_DRIZZLE = 300,
  DRIZZLE = 301,
  HEAVY_INTENSITY_DRIZZLE = 302,
  LIGHT_INTENSITY_DRIZZLE_RAIN = 310,
  DRIZZLE_RAIN = 311,
  HEAVY_INTENSITY_DRIZZLE_RAIN = 312,
  SHOWER_RAIN_AND_DRIZZLE = 313,
  HEAVY_SHOWER_RAIN_AND_DRIZZLE = 314,
  SHOWER_DRIZZLE = 321,

  // Group 5xx: Rain
  LIGHT_RAIN = 500,
  MODERATE_RAIN = 501,
  HEAVY_INTENSITY_RAIN = 502,
  VERY_HEAVY_RAIN = 503,
  EXTREME_RAIN = 504,
  FREEZING_RAIN = 511,
  LIGHT_INTENSITY_SHOWER_RAIN = 520,
  SHOWER_RAIN = 521,
  HEAVY_INTENSITY_SHOWER_RAIN = 522,
  RAGGED_SHOWER_RAIN = 531,

  // Group 6xx: Snow
  LIGHT_SNOW = 600,
  SNOW = 601,
  HEAVY_SNOW = 602,
  SLEET = 611,
  LIGHT_SHOWER_SLEET = 612,
  SHOWER_SLEET = 613,
  LIGHT_RAIN_AND_SNOW = 615,
  RAIN_AND_SNOW = 616,
  LIGHT_SHOWER_SNOW = 620,
  SHOWER_SNOW = 621,
  HEAVY_SHOWER_SNOW = 622,

  // Group 7xx: Atmosphere
  MIST = 701,
  SMOKE = 711,
  HAZE = 721,
  SAND_DUST_WHIRLS = 731,
  FOG = 741,
  SAND = 751,
  DUST = 761,
  VOLCANIC_ASH = 762,
  SQUALLS = 771,
  TORNADO = 781,

  // Group 800: Clear
  CLEAR_SKY = 800,

  // Group 80x: Clouds
  FEW_CLOUDS_11_25 = 801,
  SCATTERED_CLOUDS_25_50 = 802,
  BROKEN_CLOUDS_51_84 = 803,
  OVERCAST_CLOUDS_85_100 = 804,
};

// Can contain current weather data or hourly forecast
struct Condition {
  std::time_t date{0};
  std::time_t sunrise{0};
  std::time_t sunset{0};
  long double temperature;
  long double felt_temperature;
  int pressure;
  int humidity;
  long double uv_index;
  int clouds;
  int visibility;
  long double probability_of_precipitation;
  long double wind_speed;
  int wind_degree;
  long double wind_gust;

  Weather weather{CLEAR_SKY};
  std::string weather_description;
  std::string weather_icon_name;
  long double rain;
  long double snow;
};

// Can contain daily forecast
struct DailyCondition {
  std::time_t date{0};
  std::time_t sunrise{0};
  std::time_t sunset{0};
  std::time_t moonrise{0};
  std::time_t moonset{0};
  long double moon_phase;
  int pressure;
  int humidity;
  long double dew_point;
  long double wind_speed;
  int wind_degree;
  long double wind_gust;
  int clouds;
  long double probability_of_precipitation;
  long double uv_index;
  long double rain;
  long double snow;

  Weather weather{CLEAR_SKY};
  std::string weather_description;
  std::string weather_icon_name;
  long double temperature_day{NAN};
  long double temperature_min{NAN};
  long double temperature_max{NAN};
  long double temperature_night{NAN};
  long double temperature_evening{NAN};
  long double temperature_morning{NAN};
  long double felt_temperature_day{NAN};
  long double felt_temperature_night{NAN};
  long double felt_temperature_evening{NAN};
  long double felt_temperature_morning{NAN};
};

struct Alert {
  std::string sender;
  std::string event;
  std::time_t start_date;
  std::time_t end_date;
  std::string description;
};

struct Model {
  std::string source;
  UnitSystem unit_system{standard};
  std::time_t refresh_date{0};
  Location location;

  std::optional<Condition> current_condition;
  std::vector<Condition> hourly_forecast;
  std::vector<DailyCondition> daily_forecast;

  std::vector<Alert> alerts;

  std::list<Location> location_history;
};
} // namespace taranis
