# Taranis

> Taranis est un dieu du Ciel et de l'Orage de la mythologie celtique
> gauloise.

Local weather data on your [PocketBook](https://pocketbook.ch/en-ch)
e-book reader.

![Hourly forecast screenshot](./docs/screenshot-hourly-forecast.jpg) ![Daily forecast screenshot](./docs/screenshot-daily-forecast.jpg)

![Application running on Vivlio reader](./docs/application-running-vivlio-reader.jpg)

## Features 🥳

* Current weather

* 8 daily forecasts with sunrise hour, morning, day and evening
  temperatures, sunset hour, wind speed, humidity and precipitations.

* 24 hourly forecasts with temperature, wind speed, humidity,
  temperature curve and precipitations histogram
  
* Government weather alerts
  
* Configuration editor for system of units, custom API key, etc.

* History of locations

* Translations (English, Czech, French, Polish)

Weather data is provided by [OpenWeather](https://openweather.co.uk).

Positive feedback were received for Vivilo Touch HD Plus, PocketBook
Touch HD 3, InkPad 3 and InkPad Color 2 (The application should be
compatible with hardware running software version ≥6).

Consult the [NEWS file](NEWS.md) for notable changes between versions.

### Privacy policy

Note that the only external service called by Taranis is provided by
[OpenWeather](https://openweather.co.uk) and [their privacy
policy](https://openweather.co.uk/privacy-policy) when one is using
their APIs is quite clear:

>  Please note, that we do not collect or store any parameters from
>  your API requests, such as locations/ZIP-codes/city names, units
>  measurements, data format, etc. When you make API requests, we also
>  do not collect or store your IP address, user agent and other API
>  request parameters.

## Install

1. Download the `taranis.zip` archive from the [latest
   release](https://github.com/orontee/taranis/releases/latest).
   
2. Extract the file `taranis.app` from the archive and copy that file
   to the `applications` directory of the e-book reader using a USB
   cable.

Optionally, a custom icon can be associated to the application;
Instructions are given in [Desktop
integration](./docs/desktop_integration.md).

## Configuration

Note that the Openweather API key distributed with the application is
rate limited. A custom API key can be specified through the
"Parameters…" menu item (See [OpenWeather](https://openweathermap.org)
for instructions on how to register and generate API keys).

## Contributing

Instructions to install an unreleased version, debug, etc. can be
found in the [CONTRIBUTING file](./CONTRIBUTING.md).
