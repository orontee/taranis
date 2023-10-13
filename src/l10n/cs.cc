#include "../l10n.h"

namespace taranis {
namespace l10n {

// Czech translations

const Translations cs_translations{
    // about.cc.in
    {"This program is provided without any warranty. For more details, "
     "read GNU General Public License, version 3 or later",
     "Tento program je bez jakékoliv záruky. Další podrobnosti jsou"
     "v GNU General Public License, verze 3 nebo novější."},
    {"Credits", "Poděkování"},

    // alerts.h
    {"Alert", "Výstraha"},
    {"Start", "Začátek"},
    {"Duration", "Přes"},
    {"Source", "Zdroj"},
    {"Next alert", "Následující výstraha"},

    // app.h
    {"Unsupported software version", "Nepodporovaná verze programu"},
    {"The application isn't compatible with the software version "
     "of this reader.",
     "Aplikace není kompatibilní s programovou verzí "
     "této čtečky."},
    {"Network error", "Chyba sítě"},
    {"Failure while fetching weather data. Check your network connection.",
     "Chyba při stahování dat o počasí. Zkontroluj "
     "síťové připojení."},
    {"Service unavailable", "Služba není dostupná"},
    {"Invalid input", "Neplatné zadání"},
    {"Make sure you enter a town then a country separated by a comma.",
     "Zkontroluj, zda je správné jméno města a potom státu"
     "oddělené čárkou."},

    // currentconditionbox.h
    {"Felt", "Pocitová teplota"},

    // errors.h
    {"Location unknown to the service providing weather data. "
     "Check spelling.",
     "Lokalita není známá ve službě počasí. "
     "Zkontroluj zápis."},
    {"Sorry, an unexpected error was encountered. Report this to "
     "the application author.",
     "Chyba aplikace. Informuj autora."},
    {"An API key is required to consume weather data. Contact the "
     "application maintainer.",
     "Pro odběr daných dat o počasí je vyžadován klíč API. "
     "Kontaktuj autora aplikace."},
    {"Weather data can't be accessed with the provided API key.",
     "Data o počasí nejsou dostupná s tímto klíčem API."},
    {"The service providing weather data isn't working as "
     "expected. Retry later.",
     "Služba poskytující data o počasí nepracuje správně"
     "Zkus později."},

    // locationlist.cc
    {"Longitude", "Zem. délka"},
    {"Latitude", "Zem. šířka"},

    // menu.h
    {"Standard", "SI"},
    {"Metric", "Metrické"},
    {"Imperial", "Imperiální"},
    {"Refresh", "Aktualizuj"},
    {"Locations", "Lokality"},
    {"Units", "Jednotky"},
    {"About…", "O aplikaci"},
    {"Quit", "Konec"},
    {"Add to favorites", "Přidat do oblíbených"},
    {"Remove from favorites", "Odebrat z oblíbených"},
    {"Empty", "Prázdný"},

    // statusbar.h
    {"Ongoing update…", "Probíhá aktualizace…"},
    {"Last update:", "Poslední aktualizace:"},
    {"Weather data from", "Data o počasí z"},

    // ui.h
    {"Sunrise", "Východ slunce"},
    {"Sunset", "Západ slunce"},
    {"Pressure", "Tlak"},
    {"Humidity", "Vlhkost"},
    {"UV index", "Index UV"},
    {"Visibility", "Viditelnost"},
    {"Wind", "Vítr"},
    {"Gust", "Nárazový vítr"},
};
} // namespace l10n
} // namespace taranis
