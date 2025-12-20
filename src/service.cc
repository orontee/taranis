#include "service.h"

#include "openmeteo.h"
#include "openweather.h"

namespace taranis {

std::unique_ptr<Service> Service::by_name(DataProvider data_provider,
                                          std::shared_ptr<HttpClient> client) {
  if (data_provider == DataProvider::openmeteo) {
    return std::unique_ptr<Service>{new OpenMeteoService(client)};
  } else if (data_provider == DataProvider::openweather) {
    return std::unique_ptr<Service>{new OpenWeatherService(client)};
  }
  return std::unique_ptr<Service>{new OpenMeteoService(client)};
}
} // namespace taranis
