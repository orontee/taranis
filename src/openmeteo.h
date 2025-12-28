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

#include "boost/log/trivial.hpp"
#include "http.h"
#include "model.h"
#include "service.h"

using namespace std::string_literals;

namespace taranis {

class OpenMeteoService : public Service {
public:
  OpenMeteoService(std::shared_ptr<HttpClient> client,
                   const DataProvider &data_provider)
      : Service{client, ""}, data_provider{data_provider} {}

  DataProvider get_data_provider() override { return this->data_provider; };

  std::vector<Location> search_location(const std::string &town,
                                        const std::string &country) override;

  Location get_location() const { return this->location; }

  void set_location(const Location &location) override;

  void fetch_data(const std::string &language,
                  const std::string &units) override;

  Condition get_current_condition() const override;

  std::vector<Condition> get_hourly_forecast() const override;

  std::vector<DailyCondition> get_daily_forecast() const override;

  std::vector<Alert> get_alerts() const override;

private:
  const DataProvider data_provider;

  Json::Value returned_value;

  std::string encode_location(const std::string &town,
                              const std::string &country) const;

  Json::Value request_geocoding_api(const std::string &town,
                                    const std::string &country);

  Json::Value request_weather_forecast_api(const std::string &language,
                                           const std::string &units);

  static Condition extract_hourly_condition(const Json::Value &value);

  static DailyCondition extract_daily_condition(const Json::Value &value);

  static std::vector<Alert> extract_alerts(const Json::Value &value);
};
} // namespace taranis
