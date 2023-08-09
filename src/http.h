#pragma once

#include <curl/curl.h>
#include <curl/easy.h>
#include <json/json.h>
#include <memory>
#include <string>

#include <inkview.h>

namespace taranis {

  size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

  class HttpClient {
  public:
    Json::Value get(const std::string &url) {
      this->ensure_network();

      std::unique_ptr<CURL, void(*)(CURL*)> curl{curl_easy_init(), &curl_easy_cleanup};
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

      curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());

      std::string response_data;
      curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response_data);

      const CURLcode code = curl_easy_perform(curl.get());
      if (code == CURLE_OK) {
        long response_code;
        curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &response_code);

        switch (response_code) {
          case 200: {
            Json::Value root;
            Json::Reader reader;
            if (reader.parse(response_data, root)) {
              return root;
            }
          }
          case 400:
            throw std::runtime_error("Bad request");
          case 401:
            throw std::runtime_error("Unauthenticated");
          case 402:
            throw std::runtime_error("Payment required");
          case 403:
            throw std::runtime_error("Forbidden.");
          case 404:
            throw std::runtime_error("Not found.");
          case 500:
            throw std::runtime_error("Internal server error");
          case 501:
            throw std::runtime_error("Not implemented");
          case 502:
            throw std::runtime_error("Bad gateway");
          case 503:
            throw std::runtime_error("Service unavailable");
          default:
            throw std::runtime_error("HTML Error Code" +
                                     std::to_string(response_code));
        }
      } else {
        throw std::runtime_error("Curl Error Code " + std::to_string(code) + " " +
                                 url);
      }
    }

  private:
    void ensure_network() {
      iv_netinfo *netinfo = NetInfo();
      if (netinfo->connected) {
        ShowHourglassForce();
        return;
      }

      const char *network_name = nullptr;
      int result = NetConnect2(network_name, 1);
      if (result != 0) {
        throw std::runtime_error("Could not connect to the internet.");
      }

      netinfo = NetInfo();
      if (netinfo->connected) {
        ShowHourglassForce();
        return;
      }

      throw std::runtime_error("Could not connect to the internet.");
    }
  };
}
