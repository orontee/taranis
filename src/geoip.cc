#include "geoip.h"

#include <boost/log/trivial.hpp>

#include "errors.h"

namespace taranis {

namespace ipapiis_helpers {
  const std::string url{"https://api.ipapi.is"};
}

std::optional<Location> GeoIPService::search_location() const {
    BOOST_LOG_TRIVIAL(info)
      << "Will search location using ipapi.is and device IP";

  try {
    auto returned_value = this->client->get(ipapiis_helpers::url);
    if (returned_value.isMember("city") and
        returned_value.isMember("lon") and
        returned_value.isMember("lat")) {
      const auto lon = returned_value.get("lon", 0.0).asDouble();
      const auto lat = returned_value.get("lat", 0.0).asDouble();
      const std::string city = returned_value.get("city", "").asString();
      const std::string country = returned_value.get("country", "").asString();
      const std::string state = returned_value.get("state", "").asString();
      const Location location{lon, lat, city, country, state};

      BOOST_LOG_TRIVIAL(info)
        << "Location set to " << location.name
        << (not location.country.empty() ? std::string{", "} + location.country : std::string{})
        << (not location.state.empty() ? std::string{", "} + location.state : std::string{});

      return location;
    }
  } catch (const HttpError& error) {}
  return std::nullopt;
}

}
