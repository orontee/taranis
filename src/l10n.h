#pragma once

#include <boost/log/trivial.hpp>
#include <cstring>
#include <inkview.h>
#include <map>
#include <string>

using Translations = std::map<std::string, std::string>;

namespace taranis {

namespace l10n {
extern const Translations cs_translations;
extern const Translations de_translations;
extern const Translations fr_translations;
extern const Translations it_translations;
extern const Translations pl_translations;
extern const Translations ru_translations;
extern const Translations sk_translations;
extern const Translations uk_translations;

} // namespace l10n

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
      BOOST_LOG_TRIVIAL(warning)
          << "Found " << translations->size() << " translations to install";
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
  std::string language;

  const Translations *select_translations(const char *language) const {
    if (std::strcmp(language, "cs") == 0) {
      return &(l10n::cs_translations);
    } else if (std::strcmp(language, "de") == 0) {
      return &(l10n::de_translations);
    } else if (std::strcmp(language, "fr") == 0) {
      return &(l10n::fr_translations);
    } else if (std::strcmp(language, "it") == 0) {
      return &(l10n::it_translations);
    } else if (std::strcmp(language, "pl") == 0) {
      return &(l10n::pl_translations);
    } else if (std::strcmp(language, "ru") == 0) {
      return &(l10n::ru_translations);
    } else if (std::strcmp(language, "sk") == 0) {
      return &(l10n::sk_translations);
    } else if (std::strcmp(language, "uk") == 0) {
      return &(l10n::uk_translations);
    }
    return nullptr;
  }
};

} // namespace taranis
