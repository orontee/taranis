#pragma once

#include <cstring>
#include <inkview.h>

namespace taranis {
inline void initialize_translations() {
  LoadLanguage(currentLang());

  if (std::strcmp(currentLang(), "fr") == 0) {
    // French translations

    // about.cc.in
    AddTranslation(
        "This program is provided without any warranty. For more details, "
        "read GNU General Public License, version 3 or later",
        "Ce programme est fourni sans aucune garantie. Pour plus de détails, "
        "lisez la License publique générale GNU, version 3 ou ultérieure.");

    // app.h
    AddTranslation("Network error", "Erreur réseau");
    AddTranslation(
        "Failure while fetching weather data. Check your network connection.",
        "Échec lors du rapatriement des données météorologiques. Vérifier la "
        "connexion réseau.");
    AddTranslation("Service unavailable", "Service indisponible");
    AddTranslation("Invalid input", "Saisie invalide");
    AddTranslation(
        "Make sure you enter a town then a country separated by a comma.",
        "Assurez-vous d'avoir saisi des noms de ville et de pays séparés par "
        "une virgule.");

    // currentconditionbox.h
    AddTranslation("Felt", "Ressenti");

    // errors.h
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

    // menu.h
    AddTranslation("Standard", "Standard");
    AddTranslation("Metric", "Système métrique");
    AddTranslation("Imperial", "Unités impériales");
    AddTranslation("Refresh", "Mettre à jour");
    AddTranslation("Units", "Unités");
    AddTranslation("About…", "À propos…");
    AddTranslation("Quit", "Quitter");

    // statusbar.h
    AddTranslation("Ongoing update…", "Mise à jour en cours…");
    AddTranslation("Updated at", "Mis à jour à ");
    AddTranslation("Weather data from", "Données météorologiques de");

    // ui.h
    AddTranslation("Sunrise", "Lever");
    AddTranslation("Sunset", "Coucher");
    AddTranslation("Pressure", "Pression");
    AddTranslation("Humidity", "Humidité");
    AddTranslation("UV index", "Index UV");
    AddTranslation("Visibility", "Visibilité");
    AddTranslation("Wind", "Vent");
    AddTranslation("Gust", "Rafale");
  } else if (std::strcmp(currentLang(), "de") == 0) {
    // German translations

    // currentconditionbox.h
    AddTranslation("Felt", "Gefühlt");

    // statusbar.h
    AddTranslation("Ongoing update…", "Laufendes Update…");
    AddTranslation("Updated at", "Aktualisiert am");
    AddTranslation("Weather data from", "Wetterdaten von");
  }
}
} // namespace taranis
