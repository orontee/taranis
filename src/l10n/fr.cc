#include "../l10n.h"

namespace taranis {
// French translations

const L10n::Translations L10n::fr_translations{
    // about.cc.in
    {"This program is provided without any warranty. For more details, "
     "read GNU General Public License, version 3 or later",
     "Ce programme est fourni sans aucune garantie. Pour plus de détails, "
     "lisez la License publique générale GNU, version 3 ou ultérieure."},
    {"Credits", "Crédits"},

    // alerts.h
    {"Alert", "Alerte"},
    {"Start", "Début"},
    {"Duration", "Durée"},
    {"Source", "Source"},
    {"Next alert", "Alerte suivante"},

    // app.h
    {"Unsupported software version", "Logiciel incompatible"},
    {"The application isn't compatible with the software version "
     "of this reader.",
     "L'application n'est pas compatible avec la version "
     "logicielle de ce matériel."},
    {"Network error", "Erreur réseau"},
    {"Failure while fetching weather data. Check your network connection.",
     "Échec lors du rapatriement des données météorologiques. Vérifier la "
     "connexion réseau."},
    {"Service unavailable", "Service indisponible"},
    {"Invalid input", "Saisie invalide"},
    {"Make sure you enter a town then a country separated by a comma.",
     "Assurez-vous d'avoir saisi des noms de ville et de pays séparés par "
     "une virgule."},

    // currentconditionbox.h
    {"Felt", "Ressenti"},

    // errors.h
    {"Location unknown to the service providing weather data. "
     "Check spelling.",
     "Localité inconnue du fournisseur de données "
     "météorologiques. Vérifier l'orthographe."},
    {"Sorry, an unexpected error was encountered. Report this to "
     "the application author.",
     "Désolé, une erreur inattendue est survenue. Contacter le "
     "responsable de l'application."},
    {"An API key is required to consume weather data. Contact the "
     "application maintainer.",
     "Une clé d'API est requise pour consommer les données météorologiques. "
     "Contacter le responsable de l'application."},
    {"Weather data can't be accessed with the provided API key.",
     "La clé d'API ne permet pas de consulter les données météorologiques."},
    {"The service providing weather data isn't working as "
     "expected. Retry later.",
     "Le fournisseur de données météorologiques est "
     "hors-service. Réessayer plus tard."},

    // locationlist.cc
    {"Longitude", "Longitude"},
    {"Latitude", "Latitude"},

    // menu.h
    {"Standard", "Standard"},
    {"Metric", "Système métrique"},
    {"Imperial", "Unités impériales"},
    {"Refresh", "Mettre à jour"},
    {"Locations", "Lieux"},
    {"Units", "Unités"},
    {"About…", "À propos…"},
    {"Quit", "Quitter"},
    {"Add to favorites", "Ajouter aux favoris"},
    {"Remove from favorites", "Retirer des favoris"},
    {"Empty", "Vide"},

    // statusbar.h
    {"Ongoing update…", "Mise à jour en cours…"},
    {"Last update:", "Mis à jour :"},
    {"Weather data from", "Données météorologiques de"},

    // ui.h
    {"Sunrise", "Lever"},
    {"Sunset", "Coucher"},
    {"Pressure", "Pression"},
    {"Humidity", "Humidité"},
    {"UV index", "Index UV"},
    {"Visibility", "Visibilité"},
    {"Wind", "Vent"},
    {"Gust", "Rafale"},
};
} // namespace taranis
