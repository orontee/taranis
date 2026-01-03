#include "service.h"

#include "openmeteo.h"
#include "openweather.h"

namespace taranis {

std::unique_ptr<Service> Service::by_name(DataProvider data_provider,
                                          std::shared_ptr<HttpClient> client) {
  if (data_provider == DataProvider::openmeteo_best_match or
      data_provider == DataProvider::openmeteo_ecmwf_ifs_hres or
      data_provider == DataProvider::openmeteo_kma_seamless or
      data_provider == DataProvider::openmeteo_meteofrance_seamless or
      data_provider == DataProvider::openmeteo_knmi_seamless or
      data_provider == DataProvider::openmeteo_dmi_seamless or
      data_provider == DataProvider::openmeteo_ukmo_seamless or
      data_provider == DataProvider::openmeteo_meteoswiss_icon_seamless or
      data_provider == DataProvider::openmeteo_gem_seamless or
      data_provider == DataProvider::openmeteo_metno_seamless or
      data_provider == DataProvider::openmeteo_italia_meteo_arpae_icon_2i or
      data_provider == DataProvider::openmeteo_gfs_seamless or
      data_provider == DataProvider::openmeteo_jma_seamless or
      data_provider == DataProvider::openmeteo_bom_access_global) {
    return std::unique_ptr<Service>{
        new OpenMeteoService(client, data_provider)};
  } else if (data_provider == DataProvider::openweather) {
    return std::unique_ptr<Service>{new OpenWeatherService(client)};
  }
  return std::unique_ptr<Service>{
      new OpenMeteoService(client, DataProvider::openmeteo_best_match)};
}
} // namespace taranis
