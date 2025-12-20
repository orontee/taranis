#include "openmeteo.h"

#include "experimental/optional"
#include <boost/log/trivial.hpp>

#include "util.h"

using namespace std::chrono_literals;
template <class T> using optional = std::experimental::optional<T>;

namespace taranis {

namespace openmeteo_helpers {
const std::string forecast_url{"https://api.open-meteo.com/v1/forecast"};
const std::string geocoding_url{
    "https://geocoding-api.open-meteo.com/v1/search"};

Weather convert_to_weather_code(int weather_code) {
  switch (weather_code) {
  case 0:
    return Weather::CLEAR_SKY;
  case 1:
    return Weather::FEW_CLOUDS_11_25;
  case 2:
    return Weather::SCATTERED_CLOUDS_25_50;
  case 3:
    return Weather::OVERCAST_CLOUDS_85_100;
  case 45:
  case 48:
    // Fog and depositing rime fog
    return Weather::FOG;
  case 51:
    return Weather::LIGHT_INTENSITY_DRIZZLE;
  case 53:
    return Weather::DRIZZLE;
  case 55:
    return Weather::HEAVY_INTENSITY_DRIZZLE;
  case 56:
    return Weather::DRIZZLE_RAIN;
  case 57:
    return Weather::HEAVY_INTENSITY_DRIZZLE_RAIN;
    ;
  case 61:
    return Weather::LIGHT_RAIN;
  case 63:
    return Weather::MODERATE_RAIN;
  case 65:
    return Weather::HEAVY_INTENSITY_RAIN;
  case 66:
  case 67:
    // Freezing rain: Light and dense intensity
    return Weather::FREEZING_RAIN;
  case 71:
    return Weather::LIGHT_SNOW;
  case 73:
    return Weather::SNOW;
  case 75:
    return Weather::HEAVY_SNOW;
  case 77:
    return Weather::SLEET;
  case 80:
    return Weather::LIGHT_INTENSITY_SHOWER_RAIN;
  case 81:
    return Weather::SHOWER_RAIN;
  case 82:
    return Weather::HEAVY_INTENSITY_SHOWER_RAIN;
  case 85:
    return Weather::LIGHT_SHOWER_SNOW;
  case 86:
    return Weather::HEAVY_SHOWER_SNOW;
  case 95:
    return Weather::THUNDERSTORM;
  case 96:
    return Weather::THUNDERSTORM_WITH_LIGHT_DRIZZLE;
  case 99:
    return Weather::THUNDERSTORM_WITH_HEAVY_DRIZZLE;
  }
  return Weather::CLEAR_SKY;
}

std::string convert_to_description(int weather_code) {
  switch (weather_code) {
  case 0:
    return GetLangText("Clear sky");
  case 1:
    return GetLangText("Mainly clear sky");
  case 2:
    return GetLangText("Partly cloudy sky");
  case 3:
    return GetLangText("Overcast sky");
  case 45:
    return GetLangText("Fog");
  case 48:
    return GetLangText("Depositing rime fog");
  case 51:
    return GetLangText("Light drizzle");
  case 53:
    return GetLangText("Moderate drizzle");
  case 55:
    return GetLangText("Dense drizzle");
  case 56:
    return GetLangText("Light freezing drizzle");
  case 57:
    return GetLangText("Dense freezing drizzle");
  case 61:
    return GetLangText("Slight rain");
  case 63:
    return GetLangText("Moderate rain");
  case 65:
    return GetLangText("Heavy rain");
  case 66:
    return GetLangText("Light freezing rain");
  case 67:
    return GetLangText("Heavy freezing rain");
  case 71:
    return GetLangText("Slight snow fall");
  case 73:
    return GetLangText("Moderate snow fall");
  case 75:
    return GetLangText("Heavy snow fall");
  case 77:
    return GetLangText("Snow grains");
  case 80:
    return GetLangText("Slight rain showers");
  case 81:
    return GetLangText("Moderate rain showers");
  case 82:
    return GetLangText("Violent rain showers");
  case 85:
    return GetLangText("Slight snow showers");
  case 86:
    return GetLangText("Heavy snow showers");
  case 95:
    return GetLangText("Slight thunderstorm");
  case 96:
    return GetLangText("Moderate thunderstorm");
  case 99:
    return GetLangText("Thunderstorm with slight and heavy hail");
  }
  return "";
}

std::string convert_to_weather_icon_name(int weather_code) {
  switch (weather_code) {
  case 0:
    return "01d";
  case 1:
    return "02d";
  case 2:
    return "03d";
  case 3:
    return "04d";
  case 45:
    return "50d";
  case 48:
    return "50d";
  case 51:
    return "10d";
  case 53:
  case 55:
  case 56:
  case 57:
    return "09d";
  case 61:
    return "10d";
  case 63:
  case 65:
    return "09d";
  case 66:
  case 67:
  case 71:
  case 73:
  case 75:
  case 77:
    return "13d";
  case 80:
    return "09d";
  case 81:
  case 82:
    return "10d";
  case 85:
  case 86:
    return "13d";
  case 95:
  case 96:
  case 99:
    return "11d";
  }
  return "01d";
}

std::string convert_units_to_query_units(const std::string &units) {
  if (units == "standard") {
    return "&wind_speed_unit=ms";
  } else if (units == "metric") {
    return "&wind_speed_unit=ms";
  } else if (units == "imperial") {
    return "&wind_speed_unit=mph&temperature_unit=fahrenheit";
  }
  BOOST_LOG_TRIVIAL(warning) << "Unexpected units " << units;
  return {};
}
} // namespace openmeteo_helpers

namespace {
bool is_transposable_json_object(const Json::Value &input) {
  std::optional<size_t> common_size;
  for (auto it = input.begin(); it != input.end(); ++it) {
    if (not it->isArray()) {
      return false;
    }
    if (common_size == std::experimental::nullopt) {
      common_size = it->size();
    } else {
      if (*common_size != it->size()) {
        return false;
      }
    }
  }
  return true;
}

Json::Value transpose_json_object(const Json::Value &input) {
  Json::Value output(Json::arrayValue);
  size_t length = input.begin()->size();

  for (size_t i = 0; i < length; ++i) {
    Json::Value item(Json::objectValue);
    for (auto it = input.begin(); it != input.end(); ++it) {
      item[it.key().asString()] = (*it)[i];
    }
    output.append(item);
  }
  return output;
}
} // namespace

void OpenMeteoService::set_location(const Location &location) {
  if (location == this->location) {
    return;
  }
  this->location = location;
  this->returned_value = Json::Value::nullSingleton();
}

std::vector<Location>
OpenMeteoService::search_location(const std::string &town,
                                  const std::string &country) {
  BOOST_LOG_TRIVIAL(info) << "Searching location "
                          << format_location(town, country);
  if (town.empty()) {
    BOOST_LOG_TRIVIAL(warning)
        << "Won't search for location from empty string!";
    return {};
  }
  const auto value = this->request_geocoding_api(town, country);
  std::vector<Location> locations;
  for (const auto &location_value : value) {
    // See https://www.geonames.org for value properties
    const auto longitude = location_value.get("longitude", NAN).asDouble();
    const auto latitude = location_value.get("latitude", NAN).asDouble();
    if (std::isnan(longitude) or std::isnan(latitude)) {
      throw ServiceError::get_unexpected_error();
    }
    auto name = location_value.get("name", "").asString();
    auto country = location_value.get("country", "").asString();
    const auto state = location_value.get("admin1", "").asString();

    locations.emplace_back(longitude, latitude, name, country, state);
  }
  BOOST_LOG_TRIVIAL(info) << "Number of found locations: " << locations.size();

  return locations;
}

Condition OpenMeteoService::get_current_condition() const {
  const auto hourly_data =
      transpose_json_object(this->returned_value["hourly"]);
  return OpenMeteoService::extract_hourly_condition(*hourly_data.begin());
}

void OpenMeteoService::fetch_data(const std::string &language,
                                  const std::string &units) {
  BOOST_LOG_TRIVIAL(debug) << "Fetching weather data";

  if (std::isnan(this->location.longitude) or
      std::isnan(this->location.latitude)) {
    BOOST_LOG_TRIVIAL(warning)
        << "Won't fetch weather data for unknown location";
    return;
  }

  this->returned_value = this->request_weather_forecast_api(language, units);

  if (not this->returned_value.isMember("current") or
      not this->returned_value["current"].isObject()) {
    throw ServiceError::get_unexpected_error();
  }
  if (not this->returned_value.isMember("hourly") or
      not this->returned_value["hourly"].isObject()) {
    throw ServiceError::get_unexpected_error();
  }

  if (not this->returned_value.isMember("daily") or
      not this->returned_value["daily"].isObject()) {
    throw ServiceError::get_unexpected_error();
  }
}

std::vector<Condition> OpenMeteoService::get_hourly_forecast() const {
  std::vector<Condition> conditions;

  if (not is_transposable_json_object(this->returned_value["hourly"])) {
    return conditions;
  }
  conditions.reserve(Service::max_hourly_forecasts);
  const auto hourly_data =
      transpose_json_object(this->returned_value["hourly"]);
  for (auto &value : hourly_data) {
    conditions.push_back(OpenMeteoService::extract_hourly_condition(value));

    if (conditions.size() == OpenMeteoService::max_hourly_forecasts) {
      break;
    }
  }
  return conditions;
}

std::vector<DailyCondition> OpenMeteoService::get_daily_forecast() const {
  std::vector<DailyCondition> conditions;

  if (not is_transposable_json_object(this->returned_value["daily"])) {
    return conditions;
  }
  conditions.reserve(Service::max_daily_forecasts);
  const auto daily_data = transpose_json_object(this->returned_value["daily"]);
  for (auto &value : daily_data) {
    conditions.push_back(OpenMeteoService::extract_daily_condition(value));

    if (conditions.size() == OpenMeteoService::max_daily_forecasts) {
      break;
    }
  }
  return conditions;
}

std::vector<Alert> OpenMeteoService::get_alerts() const {
  if (not this->returned_value.isMember("alerts") or
      not this->returned_value["alerts"].isArray()) {
    return {};
  }
  return {};
  // TODO
}

Json::Value
OpenMeteoService::request_geocoding_api(const std::string &town,
                                        const std::string &country) {
  BOOST_LOG_TRIVIAL(debug) << "Requesting Geocoding API";

  std::stringstream url;
  const std::string language_code = currentLang();
  url << openmeteo_helpers::geocoding_url << "?"
      << "name=" << this->client->encode_query_parameter(town) << "&"
      << "language=" << language_code << "&"
      << "limit=5";

  if (not country.empty()) {
    url << "&countryCode" << this->client->encode_query_parameter(country);
  }

  const auto value = this->send_get_request(url.str());
  if ((not value.isObject()) or (not value.isMember("results")) or
      (not value["results"].isArray()) or (value["results"].size() == 0)) {
    throw ServiceError::get_unknown_location_error();
  }
  return value["results"];
}

Json::Value
OpenMeteoService::request_weather_forecast_api(const std::string &language,
                                               const std::string &units) {
  BOOST_LOG_TRIVIAL(debug) << "Requesting Weather Forcast API";

  std::stringstream url;
  url << openmeteo_helpers::forecast_url << "?"
      << "longitude=" << this->location.longitude << "&"
      << "latitude=" << this->location.latitude << "&"
      << "daily="
      << ("weather_code,temperature_2m_max,temperature_2m_min,apparent_"
          "temperature_max,apparent_temperature_min,sunrise,sunset,uv_index_"
          "max,rain_sum,snowfall_sum,precipitation_probability_mean,wind_speed_"
          "10m_max,wind_gusts_10m_max,wind_direction_10m_dominant,dew_point_2m_"
          "mean,pressure_msl_mean,visibility_mean,relative_humidity_2m_mean,"
          "cloud_cover_mean")
      << "&"
      << "hourly="
      << ("temperature_2m,apparent_temperature,cloud_cover,precipitation_"
          "probability,precipitation,pressure_msl,relative_humidity_2m,uv_"
          "index,visibility,weather_code,wind_speed_10m,wind_direction_10m,"
          "wind_gusts_10m")
      << "&"
      << "current="
      << ("temperature_2m,precipitation,weather_code,apparent_temperature,wind_"
          "speed_10m,wind_direction_10m,wind_gusts_10m")
      << "&"
      << "timeformat=unixtime" << "&"
      << "start_hour=" << now_as_iso8601_with_hour() << "&"
      << "end_hour=" << now_as_iso8601_with_hour(Service::max_hourly_forecasts)
      << "&"
      << "start_date=" << now_as_iso8601() << "&"
      << "end_date=" << now_as_iso8601(Service::max_daily_forecasts) << "&"
      << openmeteo_helpers::convert_units_to_query_units(units);

  const auto returned_value = this->send_get_request(url.str());
  return returned_value;
}

Condition OpenMeteoService::extract_hourly_condition(const Json::Value &value) {
  const TimePoint date{value.get("time", 0).asInt64() * 1s};
  const TimePoint sunrise;
  const TimePoint sunset;
  const auto temperature = value.get("temperature_2m", NAN).asDouble();
  const auto felt_temperature =
      value.get("apparent_temperature", NAN).asDouble();
  const auto pressure = value.get("pressure_msl", 0).asInt();
  const auto humidity = value.get("relative_humidity_2m", 0).asInt();
  const auto clouds = value.get("cloud_cover", 0).asInt();
  const auto visibility = value.get("visibility", 0).asInt();
  const auto probability_of_precipitation =
      value.get("precipitation_probability", NAN).asDouble() / 100;
  const auto uv_index = value.get("uv_index", NAN).asDouble();
  const auto wind_speed = value.get("wind_speed_10m", NAN).asDouble();
  const auto wind_degree = value.get("wind_direction_10m", 0).asInt();
  const auto wind_gust = value.get("wind_gusts_10m", NAN).asDouble();

  Condition condition{date,
                      sunrise,
                      sunset,
                      temperature,
                      felt_temperature,
                      pressure,
                      humidity,
                      uv_index,
                      clouds,
                      visibility,
                      probability_of_precipitation,
                      wind_speed,
                      wind_degree,
                      wind_gust};

  const auto openmeteo_weather_code = value.get("weather_code", 0).asInt();
  condition.weather =
      openmeteo_helpers::convert_to_weather_code(openmeteo_weather_code);
  condition.weather_description =
      openmeteo_helpers::convert_to_description(openmeteo_weather_code);
  condition.weather_icon_name =
      openmeteo_helpers::convert_to_weather_icon_name(openmeteo_weather_code);

  condition.rain = value.get("rain", NAN).asDouble();
  condition.snow = value.get("snowfall", NAN).asDouble();

  return condition;
}

DailyCondition
OpenMeteoService::extract_daily_condition(const Json::Value &value) {
  const TimePoint date{value.get("time", 0).asInt64() * 1s};
  const TimePoint sunrise{value.get("sunrise", 0).asInt64() * 1s};
  const TimePoint sunset{value.get("sunset", 0).asInt64() * 1s};
  const auto pressure = value.get("pressure_msl_mean", 0).asInt();
  const auto uv_index = value.get("uv_index_max", NAN).asDouble();
  const auto rain = value.get("rain_sum", NAN).asDouble();
  const auto snow = value.get("snowfall_sum", NAN).asDouble();
  const auto probability_of_precipitation =
      value.get("precipitation_probability_mean", NAN).asDouble() / 100;
  const auto wind_speed = value.get("wind_speed_10m_max", NAN).asDouble();
  const auto wind_gust = value.get("wind_gusts_10m_max", NAN).asDouble();
  const auto wind_degree = value.get("wind_direction_10m_dominant", 0).asInt();
  const auto dew_point = value.get("dew_point_2m_mean", NAN).asDouble();
  const auto clouds = value.get("cloud_cover_mean", 0).asInt();
  const auto humidity = value.get("relative_humidity_2m_mean", 0).asInt();

  const TimePoint moonrise{0s};
  const TimePoint moonset{0s};
  const double moon_phase{NAN};
  const std::string summary{};

  DailyCondition condition{date,      sunrise,    sunset,
                           moonrise,  moonset,    moon_phase,
                           summary,   pressure,   humidity,
                           dew_point, wind_speed, wind_degree,
                           wind_gust, clouds,     probability_of_precipitation,
                           uv_index,  rain,       snow};

  condition.temperature_min = value.get("temperature_2m_min", NAN).asDouble();
  condition.temperature_max = value.get("temperature_2m_max", NAN).asDouble();

  const auto openmeteo_weather_code = value.get("weather_code", 0).asInt();
  condition.weather =
      openmeteo_helpers::convert_to_weather_code(openmeteo_weather_code);
  condition.weather_description =
      openmeteo_helpers::convert_to_description(openmeteo_weather_code);
  condition.weather_icon_name =
      openmeteo_helpers::convert_to_weather_icon_name(openmeteo_weather_code);

  return condition;
}
} // namespace taranis
