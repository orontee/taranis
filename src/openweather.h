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
#include "service.h"

using namespace std::string_literals;

namespace taranis {

class OpenWeatherService : public Service {
public:
  OpenWeatherService(std::shared_ptr<HttpClient> client)
      : Service{
            client,
            "4620ad6f20069b66bc36b1e88ceb4541" // API key associated to rate
                                               // limited plan
        } {}

  DataProvider get_data_provider() override {
    return DataProvider::openweather;
  };

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
  Json::Value returned_value;

  std::string encode_location(const std::string &town,
                              const std::string &country) const;

  Json::Value request_geocoding_api(const std::string &town,
                                    const std::string &country);

  Json::Value request_onecall_api(const std::string &language,
                                  const std::string &units);

  static Condition extract_condition(const Json::Value &value);

  static DailyCondition extract_daily_condition(const Json::Value &value);

  static std::vector<Alert> extract_alerts(const Json::Value &value);
};
} // namespace taranis
