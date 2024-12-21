# News

All notable changes to this project will be documented in this file.

The format is based on [Keep a
Changelog](https://keepachangelog.com/en/1.0.0/), and this project
adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

### Changed

### Removed

## [1.11.2 - 2024-12-20]

### Changed

- Update of German and Ukrainian translations
  [#22](https://github.com/orontee/taranis/issues/22)

## [1.11.1 - 2024-12-6]

### Changed

- Fix missing translations for German, Russian and Ukrainian
  [#103](https://github.com/orontee/taranis/issues/103)

## [1.11.0 - 2024-11-24]

### Changed

- Remove power off logo when generation is disabled
  [#102](https://github.com/orontee/taranis/issues/102)

- Power off logo adapt to screen mode 
  [#101](https://github.com/orontee/taranis/issues/101)

## [1.10.1 - 2024-11-23]

### Changed

- Fix wrong computation of alert description height
  [#100](https://github.com/orontee/taranis/issues/100)

## [1.10.0 - 2024-11-20]

### Added

- Enable dark mode depending on device state (expects firmware >=
  6.8.x) [#98](https://github.com/orontee/taranis/issues/98)

- Add close button to alert dialogs
  [#99](https://github.com/orontee/taranis/issues/99)

- Add icons to sunrise and sunset lines
  [#95](https://github.com/orontee/taranis/issues/95)

- Dialog with daily conditions details
  [#16](https://github.com/orontee/taranis/issues/16)

### Changed

- Sunrise and sunset times have been transposed to match the
  presentation in new daily conditions details
  [#16](https://github.com/orontee/taranis/issues/16)

## [1.9.0 - 2024-07-09]

### Added

- Draw lines for sunrise and sunset
  [#93](https://github.com/orontee/taranis/issues/93)

### Changed

- Fix precipitation histogram possibly not showing snow due to
  incomplete check [#94](https://github.com/orontee/taranis/issues/94)

## [1.8.0 - 2024-02-21]

### Added

- Display full temperature unit
  [#91](https://github.com/orontee/taranis/issues/91)

- Data update strategy can be configured
  [#88](https://github.com/orontee/taranis/issues/88)

- Font have anti-aliasing activated
  [#89](https://github.com/orontee/taranis/issues/89)

- Serbian translations
  [#22](https://github.com/orontee/taranis/issues/22)

- CI pipeline generates a ".pbi" installer
  [#84](https://github.com/orontee/taranis/issues/84)

### Changed

- Dates in daily forecast box now are smaller to free space and
  improve readability
  [#89](https://github.com/orontee/taranis/issues/89)

- Read default font size from theme and deduce other fonts size from
  that size. This has been done to improve the UI layout on small
  screen. [#89](https://github.com/orontee/taranis/issues/89)

- Alerts button has been resized and moved to free space for current
  weather box [#86](https://github.com/orontee/taranis/issues/86)

## [1.7.0 - 2023-12-31]

### Added

- Automatic elision of long weather description
  [#81](https://github.com/orontee/taranis/issues/81)

- Row of dots to represent forecast pages 
  [#77](https://github.com/orontee/taranis/issues/77)

- Check software version
  [#79](https://github.com/orontee/taranis/issues/79)

- Daily forecast can be used as power off logo
  [#28](https://github.com/orontee/taranis/issues/28)

- Menu item to edit location
  [#68](https://github.com/orontee/taranis/issues/68)

### Changed

- Hide wind information when speed is zero
  [#82](https://github.com/orontee/taranis/issues/82)

- Resize application icon
  [#72](https://github.com/orontee/taranis/issues/72)

- Dynamically shorten status bar date to avoid screen overflow
  [#80](https://github.com/orontee/taranis/issues/80)

- Humidity was replaced by speed of wind gusts
  [#78](https://github.com/orontee/taranis/issues/78)

- Icons in daily forecast are now centered vertically
  [#76](https://github.com/orontee/taranis/issues/76)

- Switching from daily to hourly forecast now considers key or swipe
  direction to set forecast offset
  [#74](https://github.com/orontee/taranis/issues/74)

- Some unnecessary redraw are avoided to fix screen flickering
  [#73](https://github.com/orontee/taranis/issues/73)

- Alerts were displayed in a standard dialog. A widget dedicated to
  alert viewing has been implemented to improve
  readability. [#66](https://github.com/orontee/taranis/issues/66)

## [ 1.6.0 ] - 2023-11-01

### Changed

- There used to be 8 wind direction icons. To improve precision, a
  single icon is now rotated according to wind
  direction. [#63](https://github.com/orontee/taranis/issues/63)

- Fix possible missing translations
  [#64](https://github.com/orontee/taranis/issues/64)

## [ 1.5.0 ] - 2023-10-26

### Added

- Display icon for wind direction
  [#19](https://github.com/orontee/taranis/issues/19)

### Changed

- New translation infrastructure. Hopefully it should not have any
  effect visible to the end user.
  [#61](https://github.com/orontee/taranis/issues/61)

- The hourly forecast view used to display forecasts for 24 hours. It
  has been increased to 48 hours. Labels have been added to identify
  dates.
  [#62](https://github.com/orontee/taranis/issues/62)

- For whatever reason, on some devices, forecast timestamps did not
  correspond to exact hours. Those timestamps are now rounded to the
  nearest hour.
  [#32](https://github.com/orontee/taranis/issues/32)

## [1.4.0] - 2023-10-19

### Added

- Display application icon and user friendly name in task manager
  [#60](https://github.com/orontee/taranis/issues/60)

- Configuration editor 
  [#2](https://github.com/orontee/taranis/issues/2)

### Changed

- Update of translations
  [#58](https://github.com/orontee/taranis/issues/58)

## [1.3.0] - 2023-10-14

### Added

- Czech translations
  [#58](https://github.com/orontee/taranis/issues/58)

- Dump/Restore most of model in application state
  [#57](https://github.com/orontee/taranis/issues/57)

- Check flight mode state before refreshing data
  [#57](https://github.com/orontee/taranis/issues/57)

### Changed

- At application start, it used to show the same type of forecast
  (daily or hourly) as when exiting. Now the application always starts
  with the same type of forecast. The default is to start with the
  hourly forecast. Add `start_with_daily_forecast = true` to the
  configuration file to change default
  behavior. [#59](https://github.com/orontee/taranis/issues/59)

- Don't clear model in case of data refresh failure
  [#57](https://github.com/orontee/taranis/issues/57)

## [1.2.0] - 2023-10-09

### Added

- Display location list when multiple locations are returned by
  Geocoding API [#56](https://github.com/orontee/taranis/issues/56)

- Compile with O3 optimization and strip executable
  [#25](https://github.com/orontee/taranis/issues/25)

- Add log file
  [#37](https://github.com/orontee/taranis/issues/37)

- Document desktop integration
  [#36](https://github.com/orontee/taranis/issues/36)

- Extend history items with favorite boolean and make sure favorites
  won't disappear from history
  [#40](https://github.com/orontee/taranis/issues/40)

- Automatic refresh should not popup connection dialog
  [#50](https://github.com/orontee/taranis/issues/50)

- API key can be specified through configuration file
  [#44](https://github.com/orontee/taranis/issues/44)

- Automatic refresh every hour
  [#26](https://github.com/orontee/taranis/issues/26)

- Show month day and name under day name
  [#48](https://github.com/orontee/taranis/issues/48)

- Evenly distribute space between columns of daily forecast view
  [#49](https://github.com/orontee/taranis/issues/49)

- Touch control allow to switch pages
  [#27](https://github.com/orontee/taranis/issues/27)

- Min and max temperatures are displayed in daily forecast
  [#43](https://github.com/orontee/taranis/issues/43)

### Changed

- Display location country as returned by Geocoding API. Previously
  user input was shown. 
  
  ⚠️ Note that this introduces a breaking change in how the application
  used to dump its state, and favorites will be lost with this change.

  [#56](https://github.com/orontee/taranis/issues/56)

- URL encode location when requesting Geocoding API
  [#55](https://github.com/orontee/taranis/issues/55)

- Fix possible crash on missing attributes in OpenWeather API payload
  [#52](https://github.com/orontee/taranis/issues/52)

- Fix menu not opening after a refresh without internet connection
  [#53](https://github.com/orontee/taranis/issues/53)

- Don't open menu during data refresh
  [#51](https://github.com/orontee/taranis/issues/51)

- Fix some (minor) visual glitches 
  [#47](https://github.com/orontee/taranis/issues/47)

- Display full date in status bar
  [#46](https://github.com/orontee/taranis/issues/46)

- Fix background color of icons
  [#45](https://github.com/orontee/taranis/issues/45)

### Removed

- It's not possible to set default location through configuration file
  anymore [#56](https://github.com/orontee/taranis/issues/56)

## [1.1.0] - 2023-09-16

### Added

- Display daily forecast
  [#8](https://github.com/orontee/taranis/issues/8)

- Display credits in about dialog
  [#34](https://github.com/orontee/taranis/issues/34)

- History of locations in menu
  [#18](https://github.com/orontee/taranis/issues/18)

- Display alerts [#24](https://github.com/orontee/taranis/issues/24)

- Polish translations
  [#22](https://github.com/orontee/taranis/issues/22)

### Changed

- Switching forecast view should not refresh data nor update whole
  screen [#38](https://github.com/orontee/taranis/issues/38)

- Don't refresh data after cancel of location edit
  [#39](https://github.com/orontee/taranis/issues/39)

- Fix invisible alerts button handing touch events
  [#35](https://github.com/orontee/taranis/issues/35)

## [1.0.1] - 2023-09-07

### Added

- Check software version at startup
  [#30](https://github.com/orontee/taranis/issues/30)

## [1.0.0] - 2023-09-06

### Added

- Draw precipitation histogram
  [#20](https://github.com/orontee/taranis/issues/20)

- Sub-menu to select unit system
  [#4](https://github.com/orontee/taranis/issues/4)

- Location selection through text input
  [#12](https://github.com/orontee/taranis/issues/12)

- Specify language when calling OpenWeather API
  [#15](https://github.com/orontee/taranis/issues/15)

- Take profile into account when reading configuration
  [#13](https://github.com/orontee/taranis/issues/13)

- Display current weather description
  [#14](https://github.com/orontee/taranis/issues/14)

- Elide location text when too long
  [#12](https://github.com/orontee/taranis/issues/12)

- About dialog [#7](https://github.com/orontee/taranis/issues/7)

- Application menu [#6](https://github.com/orontee/taranis/issues/6)

- Support for translations
  [#3](https://github.com/orontee/taranis/issues/3)

- Weather icons [#1](https://github.com/orontee/taranis/issues/1)
Automatic refresh should not popup connection dialog
  [#50](https://github.com/orontee/taranis/issues/50)

- API key can be specified through configuration file
  [#44](https://github.com/orontee/taranis/issues/44)

- Automatic refresh every hour
  [#26](https://github.com/orontee/taranis/issues/26)

- Show month day and name under day name
  [#48](https://github.com/orontee/taranis/issues/48)

- Evenly distribute space between columns of daily forecast view
  [#49](https://github.com/orontee/taranis/issues/49)

- Touch control allow to switch pages
  [#27](https://github.com/orontee/taranis/issues/27)

- Min and max temperatures are displayed in daily forecast
  [#43](https://github.com/orontee/taranis/issues/43)

### Changed

- Fix some (minor) visual glitches 
  [#47](https://github.com/orontee/taranis/issues/47)

- Display full date in status bar
  [#46](https://github.com/orontee/taranis/issues/46)

- Fix background color of icons
  [#45](https://github.com/orontee/taranis/issues/45)
