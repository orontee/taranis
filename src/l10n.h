#pragma once

#include <boost/log/trivial.hpp>
#include <cstring>
#include <inkview.h>
#include <map>
#include <string>

namespace taranis {

typedef std::map<std::string, std::string> Translations;

namespace l10n {
const extern Translations cs_translations;
const extern Translations fr_translations;
const extern Translations pl_translations;
} // namespace l10n

inline void initialize_translations() {
  const auto current_language = currentLang();

  BOOST_LOG_TRIVIAL(debug) << "Initializing translations " << current_language;

  LoadLanguage(current_language);

  const Translations *translations{nullptr};
  if (std::strcmp(current_language, "cs") == 0) {
    translations = &l10n::cs_translations;
  } else if (std::strcmp(current_language, "fr") == 0) {
    translations = &l10n::fr_translations;
  } else if (std::strcmp(current_language, "pl") == 0) {
    translations = &l10n::pl_translations;
  }

  if (translations) {
    for (const auto &translation : l10n::fr_translations) {
      AddTranslation(translation.first.c_str(), translation.second.c_str());
    }
  }
}
} // namespace taranis
