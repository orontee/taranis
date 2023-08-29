#pragma once

#include "json/value.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <experimental/optional>
#include <json/json.h>
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
                                          const std::string &language) {
    const auto lonlat = this->identify_lonlat(town, country);

    std::stringstream url;
    url << openweather::url << openweather::onecall_path << "?"
        << "lon=" << lonlat.first << "&"
        << "lat=" << lonlat.second << "&"
        << "units="
        << "metric"
        << "&"
        << "exclude=minutely"
        << "&"
        << "lang=" << language << "&"
        << "appid=" << this->api_key;

    const auto returned_value = this->send_get_request(url.str());

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

private:
  static const int max_forecasts = 24;

  std::string api_key;
  HttpClient client;

  std::string town;
  std::string country;
  std::optional<std::pair<long double, long double>> lonlat;

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
    const auto temperature = value.get("temp", NAN).asDouble();
    const auto felt_temperature = value.get("feels_like", NAN).asDouble();
    const auto humidity = value.get("humidity", NAN).asInt();
    const auto wind_speed = value.get("wind_speed", NAN).asDouble();

    Condition condition{date, temperature, felt_temperature, humidity,
                        wind_speed};

    if (value.isMember("weather") and value["weather"].isArray() and
        value["weather"].isValidIndex(0)) {
      const auto weather_value = value["weather"][0];
      condition.weather =
          static_cast<Weather>(weather_value.get("id", CLEAR_SKY).asInt());
      condition.weather_description =
          weather_value.get("description", "").asString();
      condition.weather_icon_name = weather_value.get("icon", "").asString();
    }
    return condition;
  }
};
} // namespace taranis
