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
  Json::Value get(const std::string &url);

private:
  std::unique_ptr<CURL, void (*)(CURL *)> preprare_curl();

  void ensure_network();

  static size_t write_callback(void *contents, size_t size, size_t nmemb,
                               void *userp);
};
} // namespace taranis
