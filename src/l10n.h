#pragma once

#include <cstring>
#include <inkview.h>

namespace taranis {
  inline void initialize_translations() {
    LoadLanguage(currentLang());

    if (std::strcmp(currentLang(), "fr") == 0) {
      AddTranslation("Felt", "Ressenti");
      AddTranslation("Ongoing update…", "Mise à jour en cours…");
      AddTranslation("Updated at", "Mis à jour à ");
      AddTranslation("Weather data from", "Données météorologiques de");
    } else if (std::strcmp(currentLang(), "de") == 0) {
      AddTranslation("Felt", "Gefühlt");
      AddTranslation("Ongoing update…", "Laufendes Update…");
      AddTranslation("Updated at", "Aktualisiert am");
      AddTranslation("Weather data from", "Wetterdaten von");
    } 
  }
}
