# Taranis

> Taranis est un dieu du Ciel et de l'Orage de la mythologie celtique
> gauloise.

Local weather data on your [PocketBook](https://pocketbook.ch/en-ch)
e-book reader.

![Hourly forecast screenshot](./docs/screenshot-hourly-forecast.jpg) ![Daily forecast screenshot](./docs/screenshot-daily-forecast.jpg)

![Application running on Vivlio reader](./docs/application-running-vivlio-reader.jpg)

## Features 🥳

* Current weather

* Up to 8 daily forecasts with sunrise/sunset hour, morning, day and
  evening temperatures, wind speed, wind gust and precipitations.

* 48 hourly forecasts with temperature, wind direction, wind speed,
  wind gust, temperature curve and precipitations histogram.

* Choice of data provider: [OpenWeather](https://openweather.co.uk) or
  [Open-Meteo](https://open-meteo.com/).

* Configuration editor for system of units, custom API key, etc.

* History of locations, favorite locations.

* Generation of shutdown logo.

* Translations (Czech, French, German, Polish, Ukrainian, etc.)

Positive feedback were received for Vivilo Touch HD Plus, PocketBook
Touch HD 3, InkPad 3, InkPad Color 2, InkPad Color 3, PocketBook Verse
(The application should be compatible with hardware running software
version ≥6).

Consult the [NEWS file](NEWS.md) for notable changes between versions.

## Privacy policy

The two external services that can be called by Taranis are respectful
of privacy. Those policies can be checked on:

* [OpenWeather privacy policy](https://openweather.co.uk/privacy-policy).

* [Open-Meteo](https://open-meteo.com/en/terms#privacy)

## Install

1. Download the `taranis.pbi` file from the latest release [latest
   release](https://github.com/orontee/taranis/releases/latest).
   
2. Copy that file to the e-book reader and open the corresponding
   "book" using the library application to start the installation
   process.
   
3. After the installation, the "book" can be safely suppressed from
   the library and the device.

An alternative is to download the `taranis.zip` file and copy the
extracted files to the `applications` directory on the e-book reader.
   
A custom icon can be associated to the application launch button;
Instructions are given in [Desktop
integration](./docs/desktop_integration.md#application-icon).

## Configuration

Note that the Openweather API key distributed with the application is
rate limited. A custom API key can be specified through the
"Parameters…" menu item (See [OpenWeather](https://openweathermap.org)
for instructions on how to register and generate API keys).

There's no need for an API key to consume
[Open-Meteo](https://open-meteo.com) since Taranis uses their free
non-commercial API service.

Another parameter allows to enable/disable generation of a shutdown
logo. Instructions are given in [Desktop
integration](./docs/desktop_integration.md#shutdown-logo) on how to
setup the device to effectively display this logo on power off.

## Contributing

No developer skills are required to contribute to translations or add
support for new languages, thanks to the [Weblate
platform](https://hosted.weblate.org/projects/taranis/taranis/). More
on this topic in the [translations
documentation](./po/README.md).

Instructions to install an unreleased version, debug, etc. can be
found in the [CONTRIBUTING guide](./CONTRIBUTING.md).
