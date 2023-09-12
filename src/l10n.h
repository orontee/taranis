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
    AddTranslation("Credits", "Crédits");

    // alerts.h
    AddTranslation("Alert", "Alerte");
    AddTranslation("Start", "Début");
    AddTranslation("Duration", "Durée");
    AddTranslation("Source", "Source");
    AddTranslation("Next alert", "Alerte suivante");

    // app.h
    AddTranslation("Unsupported software version", "Logiciel incompatible");
    AddTranslation("The application isn't compatible with the software version "
                   "of this reader.",
                   "L'application n'est pas compatible avec la version "
                   "logicielle de ce matériel.");
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
                   "Check spelling.",
                   "Localité inconnue du fournisseur de données "
                   "météorologiques. Vérifier l'orthographe.");
    AddTranslation("Sorry, an unexpected error was encountered. Report this to "
                   "the application author.",
                   "Désolé, une erreur inattendue est survenue. Contacter le "
                   "responsable de l'application.");
    AddTranslation(
        "An API key is required to consume weather data. Contact the "
        "application maintainer.",
        "Une clé d'API est requise pour consommer les données météorologiques. "
        "Contacter le responsable de l'application.");
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
    AddTranslation("Locations", "Lieux");
    AddTranslation("Units", "Unités");
    AddTranslation("About…", "À propos…");
    AddTranslation("Quit", "Quitter");
    AddTranslation("Empty", "Vide");

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

  } else if (std::strcmp(currentLang(), "pl") == 0) {
    // Polish translations

    // about.cc.in
    AddTranslation(
        "This program is provided without any warranty. For more details, "
        "read GNU General Public License, version 3 or later",
        "Ta aplikacja jest dostarczona bez żadnej gwarancji. Aby poznać "
        "więcej, "
        "zapoznaj się z warunkami GNU General Public License w wersji 3 lub "
        "nowszej.");
    AddTranslation("Credits", "Kredyty");

    // alerts.h
    AddTranslation("Alert", "Alert");
    AddTranslation("Start", "Zacznij o");
    AddTranslation("Duration", "Przez");
    AddTranslation("Source", "Źródło");
    AddTranslation("Next alert", "Następne powiadomienie");

    // app.h
    AddTranslation("Unsupported software version",
                   "Nieobsługiwana wersja oprogramowania");
    AddTranslation("The application isn't compatible with the software version "
                   "of this reader.",
                   "Aplikacja nie jest zgodna z oprogramowaniem "
                   "tego czytnika.");
    AddTranslation("Network error", "Błąd połączenia");
    AddTranslation(
        "Failure while fetching weather data. Check your network connection.",
        "Wystąpił błąd podczas pobierania danych pogodowych. Sprawdź "
        "połączenie sieciowe.");
    AddTranslation("Service unavailable", "Usługa niedostępna");
    AddTranslation("Invalid input", "Wprowadzono błędne dane");
    AddTranslation(
        "Make sure you enter a town then a country separated by a comma.",
        "Upewnij się, że podano poprawnie nazwę miasta i państwa "
        "rozdzielone przecinkiem.");

    // currentconditionbox.h
    AddTranslation("Felt", "Temp. odczuwalna");

    // errors.h
    AddTranslation("Location unknown to the service providing weather data. "
                   "Check spelling.",
                   "Lokalizacja nieznana w serwisie pogodowym. "
                   "Sprawdź pisownię.");
    AddTranslation("Sorry, an unexpected error was encountered. Report this to "
                   "the application author.",
                   "Wystąpił błąd aplikacji. Poinformuj "
                   "autora.");
    AddTranslation(
        "An API key is required to consume weather data. Contact the "
        "application maintainer.",
        "Do pobierania danych pogodowych wymagany jest klucz API. "
        "Skontaktuj się z autorem aplikacji.");
    AddTranslation(
        "Weather data can't be accessed with the provided API key.",
        "Dane pogodowe nie są dostępne przy użyciu tego klucza API.");
    AddTranslation("The service providing weather data isn't working as "
                   "expected. Retry later.",
                   "Serwis udostępniający dane pogodowe nie działa zgodnie"
                   "z oczekiwaniami. Spróbuj później.");

    // menu.h
    AddTranslation("Standard", "SI");
    AddTranslation("Metric", "Metryczne");
    AddTranslation("Imperial", "Imperialne");
    AddTranslation("Refresh", "Odśwież");
    AddTranslation("Locations", "Lokalizacje");
    AddTranslation("Units", "Jednostki");
    AddTranslation("About…", "O aplikacji");
    AddTranslation("Quit", "Wyjście");
    AddTranslation("Empty", "Pusty");

    // statusbar.h
    AddTranslation("Ongoing update…", "Trwa aktualizacja…");
    AddTranslation("Updated at", "Zaktualizowano");
    AddTranslation("Weather data from", "Dane pogodowe z");

    // ui.h
    AddTranslation("Sunrise", "Wschód słońca");
    AddTranslation("Sunset", "Zachód słońca");
    AddTranslation("Pressure", "Ciśnienie");
    AddTranslation("Humidity", "Wilgotność");
    AddTranslation("UV index", "Indeks UV");
    AddTranslation("Visibility", "Widoczność");
    AddTranslation("Wind", "Wiatr");
    AddTranslation("Gust", "Porywisty wiatr");

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
