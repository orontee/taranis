#include "http.h"

size_t taranis::write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  static_cast<std::string *>(userp)->append(static_cast<char *>(contents),
                                            size * nmemb);
  return size * nmemb;
}
