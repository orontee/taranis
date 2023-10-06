#pragma once

#include <curl/curl.h>
#include <curl/easy.h>
#include <json/json.h>
#include <memory>
#include <string>

#include <inkview.h>

namespace taranis {

class HttpClient {
public:
  std::string encode_query_parameter(const std::string &parameter) const;

  Json::Value get(const std::string &url);

private:
  std::string response_data;

  std::unique_ptr<CURL, void (*)(CURL *)> prepare_curl();

  void ensure_network() const;

  static size_t write_callback(void *contents, size_t size, size_t nmemb,
                               void *userp);
};
} // namespace taranis
