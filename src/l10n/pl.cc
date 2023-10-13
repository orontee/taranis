#include "../l10n.h"

namespace taranis {
namespace l10n {

// Polish translations

const Translations pl_translations{
    // about.cc.in
    {"This program is provided without any warranty. For more details, "
     "read GNU General Public License, version 3 or later",
     "Ta aplikacja jest dostarczona bez żadnej gwarancji. Aby poznać "
     "więcej, "
     "zapoznaj się z warunkami GNU General Public License w wersji 3 lub "
     "nowszej."},
    {"Credits", "Kredyty"},

    // alerts.h
    {"Alert", "Alert"},
    {"Start", "Zacznij o"},
    {"Duration", "Przez"},
    {"Source", "Źródło"},
    {"Next alert", "Następne powiadomienie"},

    // app.h
    {"Unsupported software version", "Nieobsługiwana wersja oprogramowania"},
    {"The application isn't compatible with the software version "
     "of this reader.",
     "Aplikacja nie jest zgodna z oprogramowaniem "
     "tego czytnika."},
    {"Network error", "Błąd połączenia"},
    {"Failure while fetching weather data. Check your network connection.",
     "Wystąpił błąd podczas pobierania danych pogodowych. Sprawdź "
     "połączenie sieciowe."},
    {"Service unavailable", "Usługa niedostępna"},
    {"Invalid input", "Wprowadzono błędne dane"},
    {"Make sure you enter a town then a country separated by a comma.",
     "Upewnij się, że podano poprawnie nazwę miasta i państwa "
     "rozdzielone przecinkiem."},

    // currentconditionbox.h
    {"Felt", "Temp. odczuwalna"},

    // errors.h
    {"Location unknown to the service providing weather data. "
     "Check spelling.",
     "Lokalizacja nieznana w serwisie pogodowym. "
     "Sprawdź pisownię."},
    {"Sorry, an unexpected error was encountered. Report this to "
     "the application author.",
     "Wystąpił błąd aplikacji. Poinformuj "
     "autora."},
    {"An API key is required to consume weather data. Contact the "
     "application maintainer.",
     "Do pobierania danych pogodowych wymagany jest klucz API. "
     "Skontaktuj się z autorem aplikacji."},
    {"Weather data can't be accessed with the provided API key.",
     "Dane pogodowe nie są dostępne przy użyciu tego klucza API."},
    {"The service providing weather data isn't working as "
     "expected. Retry later.",
     "Serwis udostępniający dane pogodowe nie działa zgodnie"
     "z oczekiwaniami. Spróbuj później."},

    // locationlist.cc
    {"Longitude", "Długość geo."},
    {"Latitude", "Szerokość"},

    // menu.h
    {"Standard", "SI"},
    {"Metric", "Metryczne"},
    {"Imperial", "Imperialne"},
    {"Refresh", "Odśwież"},
    {"Locations", "Lokalizacje"},
    {"Units", "Jednostki"},
    {"About…", "O aplikacji"},
    {"Quit", "Wyjście"},
    {"Add to favorites", "Dodaj do ulubionych"},
    {"Remove from favorites", "Usuń z ulubionych"},
    {"Empty", "Pusty"},

    // statusbar.h
    {"Ongoing update…", "Trwa aktualizacja…"},
    {"Last update:", "Ostatnia aktualizacja:"},
    {"Weather data from", "Dane pogodowe z"},

    // ui.h
    {"Sunrise", "Wschód słońca"},
    {"Sunset", "Zachód słońca"},
    {"Pressure", "Ciśnienie"},
    {"Humidity", "Wilgotność"},
    {"UV index", "Indeks UV"},
    {"Visibility", "Widoczność"},
    {"Wind", "Wiatr"},
    {"Gust", "Porywisty wiatr"},
};
} // namespace l10n
} // namespace taranis
