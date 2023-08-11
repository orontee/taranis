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

    AddTranslation("Network error", "Erreur réseau");
    AddTranslation(
        "Failure while fetching weather data. Check your network connection.",
        "Échec lors du rapatriement des données météorologiques. Vérifier la "
        "connexion réseau.");
    AddTranslation("Service unavailable", "Service indisponible");
    AddTranslation("Location unknown to the service providing weather data. "
                   "Check configuration.",
                   "Localité inconnue du fournisseur de données "
                   "météorologiques. Vérifier la configuration.");
    AddTranslation("Sorry, an unexpected error was encountered. Report this to "
                   "the application author.",
                   "Désolé, une erreur inattendue est survenue. Contacter le "
                   "responsable de l'application.");
    AddTranslation(
        "An API key is required to consume weather data. Check configuration.",
        "Une clé d'API est requise pour consommer les données météorologiques. "
        "Vérifier la configuration.");
    AddTranslation(
        "Weather data can't be accessed with the provided API key.",
        "La clé d'API ne permet pas de consulter les données météorologiques.");
    AddTranslation("The service providing weather data isn't working as "
                   "expected. Retry later.",
                   "Le fournisseur de données météorologiques est "
                   "hors-service. Réessayer plus tard.");
  } else if (std::strcmp(currentLang(), "de") == 0) {
    AddTranslation("Felt", "Gefühlt");
    AddTranslation("Ongoing update…", "Laufendes Update…");
    AddTranslation("Updated at", "Aktualisiert am");
    AddTranslation("Weather data from", "Wetterdaten von");
  }
}
} // namespace taranis
