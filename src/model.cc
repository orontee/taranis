#include "model.h"

namespace taranis {
std::ostream &operator<<(std::ostream &os, DataProvider dp) {
  return os << std::to_string(static_cast<int>(dp));
}
std::ostream &operator<<(std::ostream &os, UnitSystem u) {
  return os << std::to_string(static_cast<int>(u));
}

std::ostream &operator<<(std::ostream &os, GeoIPConfig c) {
  return os << std::to_string(static_cast<int>(c));
}

std::ostream &operator<<(std::ostream &os, DataUpdateStrategy s) {
  return os << std::to_string(static_cast<int>(s));
}

std::ostream &operator<<(std::ostream &os, Weather w) {
  return os << std::to_string(static_cast<int>(w));
}

} // namespace taranis
