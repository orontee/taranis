#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <experimental/optional>
#include <list>
#include <string>
#include <vector>

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

namespace taranis {

enum DataProvider {
  openweather = 0,
  openmeteo_best_match = 1,
  openmeteo_ecmwf_ifs_hres = 2,
  openmeteo_kma_seamless = 3,
  openmeteo_meteofrance_seamless = 4,
  openmeteo_knmi_seamless = 5,
  openmeteo_dmi_seamless = 6,
  openmeteo_ukmo_seamless = 7,
  openmeteo_meteoswiss_icon_seamless = 8,
  openmeteo_gem_seamless = 9,
  openmeteo_metno_seamless = 10,
  openmeteo_italia_meteo_arpae_icon_2i = 11,
  openmeteo_gfs_seamless = 12,
  openmeteo_jma_seamless = 13,
  openmeteo_bom_access_global = 14,
};

enum UnitSystem { standard = 0, metric = 1, imperial = 2 };

enum DataUpdateStrategy {
  hourly_from_startup = 0,
  hourly_when_obsolete = 1,
  manual = 2
};

struct Location {

  Location() : longitude{NAN}, latitude{NAN} {}

  Location(double longitude, double latitude, const std::string &name,
           const std::string &country, const std::string &state)
      : longitude{longitude}, latitude{latitude}, name{name}, country{country},
        state{state} {}

  Location(const Location &other)
      : longitude{other.longitude}, latitude{other.latitude}, name{other.name},
        country{other.country}, state{other.state} {}

  double longitude;
  double latitude;
  std::string name;
  std::string country;
  std::string state;

  bool operator==(const Location &other) const {
    return this->name == other.name and this->country == other.country and
           this->state == other.state;
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
  TimePoint date;
  TimePoint sunrise;
  TimePoint sunset;
  double temperature;
  double felt_temperature;
  int pressure;
  int humidity;
  double uv_index;
  int clouds;
  int visibility;
  double probability_of_precipitation;
  double wind_speed;
  int wind_degree;
  double wind_gust;

  Weather weather;
  std::string weather_description;
  std::string weather_icon_name;
  std::string weather_secondary_description;
  double rain; // mm/h
  double snow; // mm/h
};

// Can contain daily forecast
struct DailyCondition {
  TimePoint date;
  TimePoint sunrise;
  TimePoint sunset;
  std::optional<TimePoint> moonrise;
  std::optional<TimePoint> moonset;
  std::optional<double> moon_phase;
  std::string summary;
  int pressure;
  int humidity;
  double dew_point;
  double wind_speed;
  int wind_degree;
  double wind_gust;
  int clouds;
  double probability_of_precipitation;
  double uv_index;
  double rain; // mm
  double snow; // mm

  Weather weather{CLEAR_SKY};
  std::string weather_description;
  std::string weather_icon_name;
  std::string weather_secondary_description;
  double temperature_day;
  double temperature_min;
  double temperature_max;
  std::optional<double> temperature_night;
  std::optional<double> temperature_evening;
  std::optional<double> temperature_morning;
  double felt_temperature_day;
  std::optional<double> felt_temperature_night;
  std::optional<double> felt_temperature_evening;
  std::optional<double> felt_temperature_morning;
};

struct Alert {
  std::string sender;
  std::string event;
  TimePoint start_date;
  TimePoint end_date;
  std::string description;
};

struct HistoryItem {
  Location location;
  bool favorite;
};

struct Model {
  DataProvider data_provider{openweather};
  UnitSystem unit_system{standard};
  std::optional<TimePoint> refresh_date;
  Location location;

  std::optional<Condition> current_condition = std::experimental::nullopt;
  std::vector<Condition> hourly_forecast;
  std::vector<DailyCondition> daily_forecast;

  std::vector<Alert> alerts;

  std::list<HistoryItem> location_history;

  bool display_daily_forecast{false};
  DataUpdateStrategy data_update_strategy{hourly_from_startup};
  std::optional<TimePoint> last_version_check;
};
} // namespace taranis
