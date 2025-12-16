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

#include "errors.h"
#include "http.h"
#include "model.h"

using namespace std::string_literals;

namespace taranis {

class Service {
public:
  Service(std::shared_ptr<HttpClient> client, std::string api_key)
      : client{client}, api_key{api_key} {}

  virtual ~Service() {}

  std::string get_api_key() const { return this->api_key; }

  void set_api_key(const std::string &api_key) { this->api_key = api_key; }

  virtual std::vector<Location> search_location(const std::string &town,
                                                const std::string &country) = 0;

  Location get_location() const { return this->location; }

  virtual void set_location(const Location &location) = 0;

  virtual void fetch_data(const std::string &language,
                          const std::string &units) = 0;

  virtual Condition get_current_condition() const = 0;

  virtual std::vector<Condition> get_hourly_forecast() const = 0;

  virtual std::vector<DailyCondition> get_daily_forecast() const = 0;

  virtual std::vector<Alert> get_alerts() const = 0;

protected:
  static constexpr int max_hourly_forecasts = 48;
  static constexpr int max_daily_forecasts = 8;

  std::string api_key;

  std::shared_ptr<HttpClient> client;

  Location location;

  Json::Value send_get_request(const std::string &url) {
  try {
    return this->client->get(url);
  } catch (const HttpError &error) {
    throw ServiceError::from_http_error(error);
  } catch (const JsonParseError &error) {
    throw ServiceError::get_unexpected_error();
  }
}


};
} // namespace taranis
