#include "openweather.h"

#include <boost/log/trivial.hpp>
#include <chrono>

#include "errors.h"
#include "inkview.h"
#include "util.h"

using namespace std::chrono_literals;

namespace taranis {

namespace openweather_helpers {
const std::string base_url{"https://api.openweathermap.org"};
const std::string geo_path{"/geo/1.0/direct"};
const std::string onecall_path{"/data/3.0/onecall"};
} // namespace openweather_helpers

void OpenWeatherService::set_location(const Location &location) {
  if (location == this->location) {
    return;
  }
  this->location = location;
  this->returned_value = Json::Value::nullSingleton();
}

std::vector<Location>
OpenWeatherService::search_location(const std::string &town,
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
    const auto longitude = location_value.get("lon", NAN).asDouble();
    const auto latitude = location_value.get("lat", NAN).asDouble();
    if (std::isnan(longitude) or std::isnan(latitude)) {
      throw ServiceError::get_unexpected_error();
    }
    auto name = location_value.get("name", "").asString();
    auto country = location_value.get("country", "").asString();
    const auto state = location_value.get("state", "").asString();

    locations.emplace_back(longitude, latitude, name, country, state);
  }
  BOOST_LOG_TRIVIAL(info) << "Number of found locations: " << locations.size();

  return locations;
}

Condition OpenWeatherService::get_current_condition() const {
  return extract_condition(this->returned_value["current"]);
}

void OpenWeatherService::fetch_data(const std::string &language,
                                    const std::string &units) {
  BOOST_LOG_TRIVIAL(debug) << "Fetching weather data";

  if (std::isnan(this->location.longitude) or
      std::isnan(this->location.latitude)) {
    BOOST_LOG_TRIVIAL(warning)
        << "Won't fetch weather data for unknown location";
    return;
  }
  this->returned_value = this->request_onecall_api(language, units);

  if (not this->returned_value.isMember("current") or
      not this->returned_value["current"].isObject()) {
    throw ServiceError::get_unexpected_error();
  }
  if (not this->returned_value.isMember("hourly") or
      not this->returned_value["hourly"].isArray()) {
    throw ServiceError::get_unexpected_error();
  }

  if (not this->returned_value.isMember("daily") or
      not this->returned_value["daily"].isArray()) {
    throw ServiceError::get_unexpected_error();
  }
}

std::vector<Condition> OpenWeatherService::get_hourly_forecast() const {
  std::vector<Condition> conditions;
  conditions.reserve(OpenWeatherService::max_hourly_forecasts);
  for (auto &value : this->returned_value["hourly"]) {
    conditions.push_back(OpenWeatherService::extract_condition(value));

    if (conditions.size() == OpenWeatherService::max_hourly_forecasts) {
      break;
    }
  }
  return conditions;
}

std::vector<DailyCondition> OpenWeatherService::get_daily_forecast() const {
  std::vector<DailyCondition> conditions;
  conditions.reserve(OpenWeatherService::max_daily_forecasts);
  for (auto &value : this->returned_value["daily"]) {
    conditions.push_back(OpenWeatherService::extract_daily_condition(value));

    if (conditions.size() == OpenWeatherService::max_daily_forecasts) {
      break;
    }
  }
  return conditions;
}

std::vector<Alert> OpenWeatherService::get_alerts() const {
  if (not this->returned_value.isMember("alerts") or
      not this->returned_value["alerts"].isArray()) {
    return {};
  }
  return OpenWeatherService::extract_alerts(this->returned_value["alerts"]);
}

std::string
OpenWeatherService::encode_location(const std::string &town,
                                    const std::string &country) const {
  std::string location = this->client->encode_query_parameter(town);
  if (not country.empty()) {
    location += "," + this->client->encode_query_parameter(country);
  }
  // ⚠️ this is not the usual way to encode query parameters, in
  // particular the comma must not be encoded...
  return location;
}

Json::Value
OpenWeatherService::request_geocoding_api(const std::string &town,
                                          const std::string &country) {
  BOOST_LOG_TRIVIAL(debug) << "Requesting Geocoding API";

  std::stringstream url;
  url << openweather_helpers::base_url << openweather_helpers::geo_path << "?"
      << "q=" << this->encode_location(town, country) << "&"
      << "limit=5"
      << "&"
      << "appid=" << this->api_key;

  const auto value = this->send_get_request(url.str());
  if (not value.isArray() or value.size() == 0) {
    throw ServiceError::get_unknown_location_error();
  }
  return value;
}

Json::Value OpenWeatherService::request_onecall_api(const std::string &language,
                                                    const std::string &units) {
  BOOST_LOG_TRIVIAL(debug) << "Requesting Onecall API";

  std::stringstream url;
  url << openweather_helpers::base_url << openweather_helpers::onecall_path
      << "?"
      << "lon=" << this->location.longitude << "&"
      << "lat=" << this->location.latitude << "&"
      << "units=" << units << "&"
      << "exclude=minutely"
      << "&"
      << "lang=" << language << "&"
      << "appid=" << this->api_key;

  const auto returned_value = this->send_get_request(url.str());
  return returned_value;
}

Condition OpenWeatherService::extract_condition(const Json::Value &value) {
  const TimePoint date{value.get("dt", 0).asInt64() * 1s};
  const TimePoint sunrise{value.get("sunrise", 0).asInt64() * 1s};
  const TimePoint sunset{value.get("sunset", 0).asInt64() * 1s};
  const auto temperature = value.get("temp", NAN).asDouble();
  const auto felt_temperature = value.get("feels_like", NAN).asDouble();
  const auto pressure = value.get("pressure", 0).asInt();
  const auto humidity = value.get("humidity", 0).asInt();
  const auto uv_index = value.get("uvi", NAN).asDouble();
  const auto clouds = value.get("clouds", 0).asInt();
  const auto visibility = value.get("visibility", 0).asInt();
  const auto probability_of_precipitation = value.get("pop", NAN).asDouble();
  const auto wind_speed = value.get("wind_speed", NAN).asDouble();
  const auto wind_degree = value.get("wind_deg", 0).asInt();
  const auto wind_gust = value.get("wind_gust", NAN).asDouble();

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

  if (value.isMember("weather") and value["weather"].isArray() and
      value["weather"].isValidIndex(0)) {
    const auto weather_value = value["weather"][0];
    condition.weather =
        static_cast<Weather>(weather_value.get("id", CLEAR_SKY).asInt());
    condition.weather_description =
        weather_value.get("description", "").asString();
    condition.weather_icon_name = weather_value.get("icon", "").asString();

    if (value["weather"].isValidIndex(1)) {
      const auto weather_secondary_value = value["weather"][1];
      condition.weather_secondary_description =
          weather_secondary_value.get("description", "").asString();
    }
  }

  if (value.isMember("rain") and value["rain"].isMember("1h")) {
    const auto rain_value = value["rain"];
    condition.rain = rain_value.get("1h", NAN).asDouble();
  } else {
    condition.rain = NAN;
  }

  if (value.isMember("snow") and value["snow"].isMember("1h")) {
    const auto snow_value = value["snow"];
    condition.snow = snow_value.get("1h", NAN).asDouble();
  } else {
    condition.snow = NAN;
  }
  return condition;
}

DailyCondition
OpenWeatherService::extract_daily_condition(const Json::Value &value) {
  const TimePoint date{value.get("dt", 0).asInt64() * 1s};
  const TimePoint sunrise{value.get("sunrise", 0).asInt64() * 1s};
  const TimePoint sunset{value.get("sunset", 0).asInt64() * 1s};
  const TimePoint moonrise{value.get("moonrise", 0).asInt64() * 1s};
  const TimePoint moonset{value.get("moonset", 0).asInt64() * 1s};
  const auto moon_phase = value.get("moon_phase", NAN).asDouble();
  const auto summary = value.get("summary", "").asString();
  const auto pressure = value.get("pressure", 0).asInt();
  const auto humidity = value.get("humidity", 0).asInt();
  const auto dew_point = value.get("dew_point", NAN).asDouble();
  const auto wind_speed = value.get("wind_speed", NAN).asDouble();
  const auto wind_degree = value.get("wind_deg", 0).asInt();
  const auto wind_gust = value.get("wind_gust", NAN).asDouble();
  const auto clouds = value.get("clouds", 0).asInt();
  const auto probability_of_precipitation = value.get("pop", NAN).asDouble();
  const auto uv_index = value.get("uvi", NAN).asDouble();
  const auto rain = value.get("rain", NAN).asDouble();
  const auto snow = value.get("snow", NAN).asDouble();

  DailyCondition condition{date,      sunrise,    sunset,
                           moonrise,  moonset,    moon_phase,
                           summary,   pressure,   humidity,
                           dew_point, wind_speed, wind_degree,
                           wind_gust, clouds,     probability_of_precipitation,
                           uv_index,  rain,       snow};

  if (value.isMember("weather") and value["weather"].isArray() and
      value["weather"].isValidIndex(0)) {

    const auto weather_value = value["weather"][0];
    condition.weather =
        static_cast<Weather>(weather_value.get("id", CLEAR_SKY).asInt());
    condition.weather_description =
        weather_value.get("description", "").asString();
    condition.weather_icon_name = weather_value.get("icon", "").asString();

    if (value["weather"].isValidIndex(1)) {
      const auto weather_secondary_value = value["weather"][1];
      condition.weather_secondary_description =
          weather_secondary_value.get("description", "").asString();
    }
  }

  if (value.isMember("temp")) {
    const auto temp_value = value["temp"];
    condition.temperature_day = temp_value.get("day", NAN).asDouble();
    condition.temperature_min = temp_value.get("min", NAN).asDouble();
    condition.temperature_max = temp_value.get("max", NAN).asDouble();
    condition.temperature_night = temp_value.get("night", NAN).asDouble();
    condition.temperature_evening = temp_value.get("eve", NAN).asDouble();
    condition.temperature_morning = temp_value.get("morn", NAN).asDouble();
  }

  if (value.isMember("feels_like")) {
    const auto felt_value = value["feels_like"];
    condition.felt_temperature_day = felt_value.get("day", NAN).asDouble();
    condition.felt_temperature_night = felt_value.get("night", NAN).asDouble();
    condition.felt_temperature_evening = felt_value.get("eve", NAN).asDouble();
    condition.felt_temperature_morning = felt_value.get("morn", NAN).asDouble();
  }
  return condition;
}

std::vector<Alert>
OpenWeatherService::extract_alerts(const Json::Value &value) {
  std::vector<Alert> alerts;

  for (auto &alert_value : value) {
    const Alert alert{alert_value.get("sender_name", "").asString(),
                      alert_value.get("event", "").asString(),
                      TimePoint{alert_value.get("start", 0).asInt64() * 1s},
                      TimePoint{alert_value.get("end", 0).asInt64() * 1s},
                      alert_value.get("description", "").asString()};
    alerts.push_back(alert);
  }
  return alerts;
}
} // namespace taranis
