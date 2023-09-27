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
  
* Configurable system of units

* History of locations

* Translations (English, French, Polish)

Weather data is provided by [OpenWeather](https://openweathermap.org).

Positive feedback were received for Vivilo Touch HD Plus, PocketBook
Touch HD 3, InkPad 3 and InkPad Color 2 (The application should be
compatible with hardware running software version ≥6).

Consult the [NEWS file](NEWS.md) for notable changes between versions.

## Install

1. Download the `taranis.zip` archive from the [latest
   release](https://github.com/orontee/taranis/releases/latest).
   
2. Extract the file `taranis.app` from the archive and copy that file
   to the `applications` directory of the e-book reader using a USB
   cable.

When the application is running, press the location text at the top of
the screen to enter a custom location (the _town, country_ format is
expected).

## Configuration

The application stores the user preferences (unit system, default
location, etc.) in a configuration file writen under
`system/config/taranis.cfg`.

Note that the Openweather API key distributed with the application is
rate limited. It's possible to specify a custom key using the
`api_key` keyword in the configuration file (See
[OpenWeather](https://openweathermap.org) for instructions on how to
register and generate API keys).

## Contributing

Instructions to install an unreleased version, debug, etc. can be
found in the [CONTRIBUTING file](./CONTRIBUTING.md).
