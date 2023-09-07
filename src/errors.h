#pragma once

#include <inkview.h>
#include <stdexcept>
#include <string>

#define T(x) GetLangText(x)

namespace taranis {
struct ConnectionError : std::runtime_error {
  ConnectionError() : std::runtime_error{""} {}
};

// for Curl function errors (transport, protocol, etc. errors)
struct RequestError : std::runtime_error {
  long long code;

  RequestError(long long code) : std::runtime_error{""}, code{code} {}
};

struct HttpError : std::runtime_error {
  long code;

  HttpError(long code) : std::runtime_error{""}, code{code} {}
};

struct JsonParseError : std::runtime_error {
  JsonParseError() : std::runtime_error{""} {}
};

struct ServiceError : std::runtime_error {

  // what can be displayed to the end user, thus it must be ready
  // for translation
  ServiceError(std::string what) : std::runtime_error{what} {}

  static ServiceError get_unknown_location_error() {
    return {T("Location unknown to the service providing weather data. Check "
              "spelling.")};
  }

  static ServiceError get_unexpected_error() {
    return {T("Sorry, an unexpected error was encountered. Report this to the "
              "application author.")};
  }

  static ServiceError from_http_error(const HttpError &error) {
    switch (error.code) {
    case 400:
      return ServiceError::get_unexpected_error();
      break;
    case 401:
      return {T("An API key is required to consume weather data. Contact the "
                "application maintainer.")};
      break;
    case 402: // payment required
    case 403:
      return {T("Weather data can't be accessed with the provided API key.")};
      break;
    case 404:
      return ServiceError::get_unknown_location_error();
      break;
    case 500: // internal server error
    case 501: // not implemented
    case 502: // bad gateway
    case 503: // service unavailable
    default:
      return {T("The service providing weather data isn't working as expected. "
                "Retry later.")};
    }
  }
};

struct InvalidLocation : std::runtime_error {
  InvalidLocation() : std::runtime_error{""} {}
};

struct UnsupportedUnitSystem : std::runtime_error {
  UnsupportedUnitSystem() : std::runtime_error{""} {}
};

struct UnsupportedSoftwareVersion : std::runtime_error {
  UnsupportedSoftwareVersion() : std::runtime_error{""} {}
};
} // namespace taranis
