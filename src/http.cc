#include "http.h"

#include <boost/log/trivial.hpp>
#include <cmath>
#include <memory>
#include <regex>
#include <sstream>

#include "errors.h"

namespace taranis {

std::string
HttpClient::encode_query_parameter(const std::string &parameter) const {
  std::shared_ptr<char> escaped{
      curl_easy_escape(nullptr, parameter.c_str(), parameter.size()),
      &curl_free};
  if (escaped == nullptr) {
    return "";
  }

  std::string value{escaped.get()};
  return value;
}

std::string obfuscate_url(std::string url) {
  std::regex appid_regex{"appid=([^&]+)"};
  std::smatch match;
  if (std::regex_search(url, match, appid_regex)) {
    const std::string& key = match[1];
    const size_t key_length = key.size();
    std::string hidden_key(key_length, 'x'); // Parentheses to
                                             // distinguish from
                                             // initializer list
                                             // syntax
    if (key_length > 3) {
      hidden_key.replace(key_length - 3, 3, key.substr(key_length - 3));
    }
    return match.prefix().str() + std::string{"appid="} + hidden_key;
  }
  return url;
}

Json::Value HttpClient::get(const std::string &url) {
  BOOST_LOG_TRIVIAL(debug) << "Sending GET request " << obfuscate_url(url);

  this->ensure_network();

  this->response_data.clear();
  this->response_data.reserve(10 * CURLOPT_BUFFERSIZE);

  auto curl = this->prepare_curl();
  curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &this->response_data);

  const CURLcode code = curl_easy_perform(curl.get());
  if (code != CURLE_OK) {
    BOOST_LOG_TRIVIAL(error)
        << "Error in GET request " << code << " " << obfuscate_url(url);
    throw RequestError{code};
  }
  long response_code;
  curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &response_code);

  if (response_code != 200) {
    BOOST_LOG_TRIVIAL(error)
        << "Unexpected response " << response_code << " " << obfuscate_url(url);
    throw HttpError{response_code};
  }

  BOOST_LOG_TRIVIAL(debug) << "Received " << this->response_data.size()
                           << " bytes";

  Json::Value root;
  Json::CharReaderBuilder reader;
  reader["collectComments"] = false;
  std::string json_errors;
  std::stringstream input_stream{this->response_data};
  try {
    if (not Json::parseFromStream(reader, input_stream, &root, &json_errors)) {
      BOOST_LOG_TRIVIAL(error)
          << "JSON parser error " << json_errors << " " << obfuscate_url(url);
    }
  } catch (const Json::Exception &error) {
    BOOST_LOG_TRIVIAL(error) << error.what();
  }
  return root;
}

std::unique_ptr<CURL, void (*)(CURL *)> HttpClient::prepare_curl() {
  std::unique_ptr<CURL, void (*)(CURL *)> curl{curl_easy_init(),
                                               &curl_easy_cleanup};

  curl_slist *headers = nullptr;
  headers = curl_slist_append(headers,
                              "Content-Type: application/json; charset=UTF-8");
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

void HttpClient::ensure_network() const {
  const auto *netinfo = NetInfo();
  if (netinfo == nullptr or not netinfo->connected) {
    BOOST_LOG_TRIVIAL(debug) << "Will try to establish connection";

    const char *network_name = nullptr;
    const auto result = NetConnect2(network_name, true);
    // This shows hourglass pointer and may popup a connection
    // dialog

    if (result != 0) {
      BOOST_LOG_TRIVIAL(warning) << "Failed to establish connection";

      throw ConnectionError{};
    }

    netinfo = NetInfo();
    if (netinfo == nullptr or not netinfo->connected) {
      BOOST_LOG_TRIVIAL(warning) << "Still not connected!";

      throw ConnectionError{};
    }
  }
}

size_t HttpClient::write_callback(void *contents, size_t size, size_t nmemb,
                                  void *userp) {
  BOOST_LOG_TRIVIAL(debug) << "Writing " << size * nmemb << " bytes to buffer";
  static_cast<std::string *>(userp)->append(static_cast<char *>(contents),
                                            size * nmemb);
  return size * nmemb;
}
} // namespace taranis
