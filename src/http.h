#pragma once

#include <curl/curl.h>
#include <curl/easy.h>
#include <json/json.h>
#include <memory>
#include <string>

#include <inkview.h>

#include "errors.h"

namespace taranis {

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

class HttpClient {
public:
  Json::Value get(const std::string &url) {
    this->ensure_network();

    auto curl = this->preprare_curl();
    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());

    std::string response_data;
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response_data);

    const CURLcode code = curl_easy_perform(curl.get());
    if (code != CURLE_OK) {
      throw RequestError{code};
    }
    long response_code;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &response_code);

    if (response_code != 200) {
      throw HttpError{response_code};
    }

    Json::Value root;
    Json::Reader reader;
    if (not reader.parse(response_data, root)) {
      throw JsonParseError{};
    }
    return root;
  }

private:
  std::unique_ptr<CURL, void (*)(CURL *)> preprare_curl() {
    std::unique_ptr<CURL, void (*)(CURL *)> curl{curl_easy_init(),
                                                 &curl_easy_cleanup};

    curl_slist *headers = nullptr;
    headers = curl_slist_append(
        headers, "Content-Type: application/json; charset=UTF-8");
    headers = curl_slist_append(headers, "X-Accept: application/json");

    curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(curl.get(), CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_USERAGENT, "taranis/0.0.1");
    curl_easy_setopt(curl.get(), CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl.get(), CURLOPT_HTTP_VERSION,
                     static_cast<long>(CURL_HTTP_VERSION_2TLS));
    curl_easy_setopt(curl.get(), CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_TCP_KEEPALIVE, 1L);

    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, write_callback);
    return curl;
  }

  void ensure_network() {
    iv_netinfo *netinfo = NetInfo();
    if (not netinfo->connected) {
      const char *network_name = nullptr;
      int result = NetConnect2(network_name, 1);
      if (result != 0) {
        throw ConnectionError{};
      }

      netinfo = NetInfo();
      if (not netinfo->connected) {
        throw ConnectionError{};
      }
    }
  }
};
} // namespace taranis
