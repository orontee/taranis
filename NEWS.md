# News

All notable changes to this project will be documented in this file.

The format is based on [Keep a
Changelog](https://keepachangelog.com/en/1.0.0/), and this project
adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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

### Removed

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
Fix some (minor) visual glitches 
  [#47](https://github.com/orontee/taranis/issues/47)

- Display full date in status bar
  [#46](https://github.com/orontee/taranis/issues/46)

- Fix background color of icons
  [#45](https://github.com/orontee/taranis/issues/45)

### Removed

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
Don't open menu during data refresh
  [#51](https://github.com/orontee/taranis/issues/51)

- Fix some (minor) visual glitches 
  [#47](https://github.com/orontee/taranis/issues/47)

- Display full date in status bar
  [#46](https://github.com/orontee/taranis/issues/46)

- Fix background color of icons
  [#45](https://github.com/orontee/taranis/issues/45)

### Removed

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

### Removed

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
Fix some (minor) visual glitches 
  [#47](https://github.com/orontee/taranis/issues/47)

- Display full date in status bar
  [#46](https://github.com/orontee/taranis/issues/46)

- Fix background color of icons
  [#45](https://github.com/orontee/taranis/issues/45)

### Removed

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
