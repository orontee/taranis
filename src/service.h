#pragma once

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
class Service {
public:
  Service() {}

  std::string get_api_key() const { return this->api_key; }

  void set_api_key(const std::string &api_key) { this->api_key = api_key; }

  std::vector<Condition> fetch_data(const std::string &town,
                                    const std::string &country) {
    std::vector<Condition> conditions;

    const auto lonlat = this->identify_lonlat(town, country);

    std::stringstream url;
    url << "https://api.openweathermap.org/data/3.0/onecall"
        << "?"
        << "lon=" << lonlat.first << "&"
        << "lat=" << lonlat.second << "&"
        << "units="
        << "metric"
        << "&"
        << "exclude=minutely"
        << "&"
        << "appid=" << this->api_key;

    auto collect_value = [&conditions](const Json::Value &value) {
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

      conditions.push_back(condition);
    };

    auto returned_value = this->get_url_safely(url.str());

    if (returned_value.isMember("current")) {
      const auto current_value = returned_value["current"];
      collect_value(current_value);
    }

    if (returned_value.isMember("hourly")) {
      for (auto value : returned_value["hourly"]) {
        collect_value(value);

        if (conditions.size() == 25) {
          // current condition plus hourly forecast
          break;
        }
      }
    }
    return conditions;
  }

private:
  std::string api_key;
  HttpClient client;

  std::pair<long double, long double>
  identify_lonlat(const std::string &town, const std::string &country) {
    std::stringstream url;
    url << "https://api.openweathermap.org/geo/1.0/direct"
        << "?"
           "q="
        << town;
    if (not country.empty()) {
      url << "," << country;
    }
    url << "&"
        << "appid=" << this->api_key;

    auto returned_value = this->get_url_safely(url.str());
    if (not returned_value.isArray() or returned_value.size() == 0) {
      throw ServiceError::get_unknown_location_error();
    }

    Json::Value first_result = returned_value[0];
    const auto lon = first_result.get("lon", NAN).asDouble();
    const auto lat = first_result.get("lat", NAN).asDouble();
    if (std::isnan(lon) or std::isnan(lat)) {
      throw ServiceError::get_unexpected_error();
    }
    return std::make_pair(lon, lat);
  }

  Json::Value get_url_safely(const std::string &url) {
    try {
      return this->client.get(url);
    } catch (const HttpError &error) {
      throw ServiceError::from_http_error(error);
    } catch (const JsonParseError &error) {
      throw ServiceError::get_unexpected_error();
    }
  }
};
} // namespace taranis
