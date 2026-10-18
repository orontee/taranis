#pragma once

#include <memory>
#ifdef WITH_SDK_6_8
#include <experimental/optional>
#else
#include <optional>
#endif

#include "http.h"
#include "model.h"

#ifdef WITH_SDK_6_8
namespace std {
using experimental::bad_optional_access;
using experimental::make_optional;
using experimental::nullopt;
using experimental::nullopt_t;
using experimental::optional;
} // namespace std
#endif

namespace taranis {

class GeoIPService {
public:

  GeoIPService(std::shared_ptr<HttpClient> client):
    client{client}
  {}

  std::optional<Location> search_location() const;

private:
  std::shared_ptr<HttpClient> client;

};
}
