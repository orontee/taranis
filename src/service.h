#pragma once

#include <algorithm>
#include <cmath>
#include <ctime>
#include <experimental/optional>
#include <json/json.h>
#include <json/value.h>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "errors.h"
#include "http.h"
#include "model.h"

using namespace std::string_literals;

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {

namespace openweather {
const std::string url{"https://api.openweathermap.org"};
const std::string geo_path{"/geo/1.0/direct"};
const std::string onecall_path{"/data/3.0/onecall"};
} // namespace openweather

class Service {
public:
  Service() {}

  std::string get_api_key() const { return this->api_key; }

  void set_api_key(const std::string &api_key) { this->api_key = api_key; }

  std::vector<Condition> fetch_conditions(const std::string &town,
                                          const std::string &country,
                                          const std::string &language,
                                          const std::string &units) {
    const auto returned_value =
        this->request_onecall_api(town, country, language, units);

    if (not returned_value.isMember("current") or
        not returned_value["current"].isObject()) {
      throw ServiceError::get_unexpected_error();
    }

    std::vector<Condition> conditions;
    conditions.reserve(1 + Service::max_forecasts);
    // current condition and hourly forecasts

    conditions.push_back(Service::extract_condition(returned_value["current"]));

    if (not returned_value.isMember("hourly") or
        not returned_value["hourly"].isArray()) {
      throw ServiceError::get_unexpected_error();
    }

    for (auto value : returned_value["hourly"]) {
      conditions.push_back(Service::extract_condition(value));

      if (conditions.size() == 1 + Service::max_forecasts) {
        break;
      }
    }

    return conditions;
  }

  const std::vector<Alert> &get_alerts() const { return this->alerts; }

private:
  static const int max_forecasts = 24;

  std::string api_key;
  HttpClient client;

  std::string town;
  std::string country;
  std::optional<std::pair<long double, long double>> lonlat;

  std::vector<Alert> alerts;

  std::pair<long double, long double>
  identify_lonlat(const std::string &town, const std::string &country) {
    if (this->town != town or this->country != country) {
      this->town = town;
      this->country = country;
      this->lonlat = {};
    }
    if (not this->lonlat) {
      this->request_lonlat();
    }
    return this->lonlat.value();
  }

  void request_lonlat() {
    std::stringstream url;
    url << openweather::url << openweather::geo_path << "?"
        << "q=" << this->town;
    if (not this->country.empty()) {
      url << "," << this->country;
    }
    url << "&"
        << "appid=" << this->api_key;

    auto returned_value = this->send_get_request(url.str());
    if (not returned_value.isArray() or returned_value.size() == 0) {
      throw ServiceError::get_unknown_location_error();
    }

    Json::Value first_result = returned_value[0];
    const auto lon = first_result.get("lon", NAN).asDouble();
    const auto lat = first_result.get("lat", NAN).asDouble();
    if (std::isnan(lon) or std::isnan(lat)) {
      throw ServiceError::get_unexpected_error();
    }
    this->lonlat = std::make_pair(lon, lat);
  }

  Json::Value request_onecall_api(const std::string &town,
                                  const std::string &country,
                                  const std::string &language,
                                  const std::string &units) {
    const auto lonlat = this->identify_lonlat(town, country);

    std::stringstream url;
    url << openweather::url << openweather::onecall_path << "?"
        << "lon=" << lonlat.first << "&"
        << "lat=" << lonlat.second << "&"
        << "units=" << units << "&"
        << "exclude=minutely"
        << "&"
        << "lang=" << language << "&"
        << "appid=" << this->api_key;

    const auto returned_value = this->send_get_request(url.str());

    this->extract_alerts(returned_value);

    return returned_value;
  }

  Json::Value send_get_request(const std::string &url) {
    try {
      return this->client.get(url);
    } catch (const HttpError &error) {
      throw ServiceError::from_http_error(error);
    } catch (const JsonParseError &error) {
      throw ServiceError::get_unexpected_error();
    }
  }

  static Condition extract_condition(const Json::Value &value) {
    const auto date = static_cast<time_t>(value.get("dt", 0).asInt());
    const auto sunrise = static_cast<time_t>(value.get("sunrise", 0).asInt());
    const auto sunset = static_cast<time_t>(value.get("sunset", 0).asInt());
    const auto temperature = value.get("temp", NAN).asDouble();
    const auto felt_temperature = value.get("feels_like", NAN).asDouble();
    const auto pressure = value.get("pressure", NAN).asInt();
    const auto humidity = value.get("humidity", NAN).asInt();
    const auto uv_index = value.get("uvi", NAN).asDouble();
    const auto clouds = value.get("clouds", NAN).asInt();
    const auto visibility = value.get("visibility", NAN).asInt();
    const auto probability_of_precipitation = value.get("pop", NAN).asDouble();
    const auto wind_speed = value.get("wind_speed", NAN).asDouble();
    const auto wind_degree = value.get("wind_deg", NAN).asInt();
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

  void extract_alerts(const Json::Value &value) {
    this->alerts.clear();

    if (value.isMember("alerts") and value["alerts"].isArray()) {
      for (auto alert_value : value["alerts"]) {
        const Alert alert{
            alert_value.get("sender_name", "").asString(),
            alert_value.get("event", "").asString(),
            static_cast<time_t>(alert_value.get("start", 0).asInt()),
            static_cast<time_t>(alert_value.get("end", 0).asInt()),
            alert_value.get("description", "").asString()};
        this->alerts.push_back(alert);
      }
    }
  }
};
} // namespace taranis
