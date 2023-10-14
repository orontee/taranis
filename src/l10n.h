#pragma once

#include <boost/log/trivial.hpp>
#include <cstring>
#include <inkview.h>
#include <map>
#include <string>

namespace taranis {

class L10n {
public:
  void initialize_translations() {
    const auto current_system_language = currentLang();
    BOOST_LOG_TRIVIAL(debug)
        << "Initializing translations for " << current_system_language;

    this->language = current_system_language;

    LoadLanguage(current_system_language);

    const Translations *translations =
        this->select_translations(current_system_language);
    if (translations) {
      for (const auto &translation : *translations) {
        AddTranslation(translation.first.c_str(), translation.second.c_str());
      }
    } else {
      BOOST_LOG_TRIVIAL(warning)
          << "Translations not found for " << current_system_language;
    }
  }

  bool is_language_obsolete() const {
    const auto current_system_language = currentLang();
    return this->language != current_system_language;
  }

  void update_translations() {
    BOOST_LOG_TRIVIAL(debug) << "Updating translations";

    if (this->is_language_obsolete()) {
      this->initialize_translations();
    } else {
      BOOST_LOG_TRIVIAL(debug) << "System language hasn't changed";
    }
  }

private:
  typedef std::map<std::string, std::string> Translations;

  static const Translations cs_translations;
  static const Translations fr_translations;
  static const Translations pl_translations;

  std::string language;

  const Translations *select_translations(const char *language) const {
    if (std::strcmp(language, "cs") == 0) {
      return &(L10n::cs_translations);
    } else if (std::strcmp(language, "fr") == 0) {
      return &(L10n::fr_translations);
    } else if (std::strcmp(language, "pl") == 0) {
      return &(L10n::pl_translations);
    }
    return nullptr;
  }
};

} // namespace taranis
