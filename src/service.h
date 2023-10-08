#pragma once

#include <algorithm>
#include <cmath>
#include <ctime>
#include <json/json.h>
#include <json/value.h>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "http.h"
#include "model.h"

using namespace std::string_literals;

namespace taranis {

class Service {
public:
  Service() {}

  std::string get_api_key() const { return this->api_key; }

  void set_api_key(const std::string &api_key) { this->api_key = api_key; }

  std::vector<Location> search_location(const std::string &town,
                                        const std::string &country);

  Location get_location() const { return this->location; }

  void set_location(const Location &location);

  void fetch_data(const std::string &language, const std::string &units);

  Condition get_current_condition() const {
    return Service::extract_condition(this->returned_value["current"]);
  }

  std::vector<Condition> get_hourly_forecast() const;

  std::vector<DailyCondition> get_daily_forecast() const;

  std::vector<Alert> get_alerts() const;

private:
  static const int max_hourly_forecasts = 24;
  static const int max_daily_forecasts = 8;

  std::string api_key{"4620ad6f20069b66bc36b1e88ceb4541"};
  // API key associated to rate limited plan

  HttpClient client;

  Location location;

  Json::Value returned_value;

  std::string encode_location(const std::string &town,
                              const std::string &country) const;

  Json::Value request_geocoding_api(const std::string &town,
                                    const std::string &country);

  Json::Value request_onecall_api(const std::string &language,
                                  const std::string &units);

  Json::Value send_get_request(const std::string &url);

  static Condition extract_condition(const Json::Value &value);

  static DailyCondition extract_daily_condition(const Json::Value &value);

  static std::vector<Alert> extract_alerts(const Json::Value &value);
};
} // namespace taranis
