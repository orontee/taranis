# Translations

Contributions welcome!

## Howto

People interested in contributing to translations should use the
corresponding [Weblate
project](https://hosted.weblate.org/projects/taranis/taranis/).

## Limitations

Note that OpenWeather APIs are partially translated: Depending on the
requested language, returned texts are translated or not. For example,
when requesting data in French, weather descriptions are translated
but alerts aren't. And Taranis has no knowledge of the possible alerts
text…

Another limitation is related to dates. They are returned in a numeric
form by OpenWeather APIs and translation of week days and month names
is performed by Taranis based on Inkview SDK facilities (C++ provides
such facilities but the corresponding locales must be installed system
wide which is not the case as far as I can see). And Inkview
translation facilities are incomplete (if not broken)... Example, in
Czech, month names aren't translated.

Inkview SDK support a limited number of languages.

## Technical considerations

To keep things simple, Taranis uses the custom translation mechanism
provided by Inkview SDK, but:

* Translatable strings are gathered in the [taranis.pot
  file](./taranis.pot).

* Translations are stored in [PO files](./) which are textual,
  editable files.

* The extraction of translatable strings and updates of PO files is
  done by the common `xgettext` utility through a `meson` target.
  
  Thus to update the pot file and translation files to match current
  source code, one must run:
  
  ```sh
  meson compile -C builddir taranis-update-po
  ```

* The build target has been extended to automatically generate a C++
  file (matching the expectations of Inkview SDK) from all PO files, see
  [generate_l10n_cc.py](../scripts/generate_l10n_cc.py).

## Credits

The list of contributors to translations is extracted from the Weblate
project. It doesn't reflect the heavy contributions made by Cyfranek
and vita5656!

### French

* Matthias Meulien <orontee@gmail.com> - 11


### German

* breiti-oi (breiti-oi) <breiti_oi@web.de> - 101


### Polish

* Radosław Uliszak (Cyfranek) <cyfranekblog@protonmail.com> - 1


### Russian

* Tancrède (Tank) <tancrede.meulien@gmail.com> - 76


### Serbian

* kakonema (kakonema) <kakonema@gmail.com> - 70


### Ukrainian

* Vitalii (stopchan) <stopchan@gmail.com> - 115
