#pragma once

#include <array>
#include <cctype>
#include <cstring>
#include <ctime>
#include <experimental/optional>
#include <inkview.h>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "config.h"
#include "errors.h"
#include "events.h"
#include "history.h"
#include "l10n.h"
#include "model.h"
#include "service.h"
#include "state.h"
#include "ui.h"
#include "units.h"
#include "util.h"

using namespace std::placeholders;
using namespace std::string_literals;

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {

void handle_about_dialog_button_clicked(int button_index);

std::string get_about_content();
// defined in generated file about.cc

class App {
public:
  App()
      : model{std::make_shared<Model>()}, service{std::make_unique<Service>()},
        application_state{std::make_unique<ApplicationState>(this->model)},
        history{std::make_unique<LocationHistoryProxy>(this->model)} {}

  int process_event(int event_type, int param_one, int param_two) {
    if (event_type == EVT_INIT) {
      this->setup();
      return 1;
    }

    if (event_type == EVT_SHOW) {
      this->show();
      return 1;
    }

    if ((event_type == EVT_EXIT) || (event_type == EVT_HIDE)) {
      this->exit();
      return 1;
    }

    if (event_type == EVT_CONFIGCHANGED) {
      this->load_config();
      return 1;
    }

    if (event_type == EVT_CUSTOM) {
      return this->handle_custom_event(param_one, param_two);
    }

    if (this->ui) {
      if (ISPOINTEREVENT(event_type)) {
        return this->ui->handle_pointer_event(event_type, param_one, param_two);
      }
      if (ISKEYEVENT(event_type)) {
        if (event_type == EVT_KEYPRESS) {
          return this->ui->handle_key_pressed(param_one);
        }
      }
    }

    return 0;
  }

private:
  static constexpr char refresh_timer_name[] = "taranis_refresh_timer";
  static constexpr int refresh_period{60 * 60 * 1000};
  static constexpr int error_dialog_delay{5000};

  std::shared_ptr<Model> model;
  std::unique_ptr<Service> service;
  std::unique_ptr<ApplicationState> application_state;
  std::unique_ptr<LocationHistoryProxy> history;
  std::unique_ptr<Ui> ui;

  std::string language;

  void setup() {
    const auto version = GetSoftwareVersion();
    try {
      check_software_version(version);
    } catch (const UnsupportedSoftwareVersion &error) {
      Message(ICON_WARNING, GetLangText("Unsupported software version"),
              GetLangText("The application isn't compatible with the software "
                          "version of this reader."),
              error_dialog_delay);
      this->exit();
      return;
    }
    this->ui = std::make_unique<Ui>(this->model);
    this->initialize_language();
    this->load_config();
  }

  void show() {
    if (not this->ui)
      return;

    this->ui->show();
  }

  void exit() {
    this->ui.reset();
    this->history.reset();
    this->application_state.reset();
    this->service.reset();
    this->model.reset();

    CloseApp();
  }

  void initialize_language() {
    this->language = currentLang();
    initialize_translations();
  }

  void load_config() {
    Config config;

    const auto api_key_from_config = config.read_string("api_key", "");
    const auto is_api_key_obsolete =
        (not api_key_from_config.empty() and
         api_key_from_config != this->service->get_api_key());
    if (is_api_key_obsolete) {
      this->service->set_api_key(api_key_from_config);
    }

    const auto unit_system_from_config = static_cast<UnitSystem>(
        config.read_int("unit_system"s, UnitSystem::metric));
    const bool is_unit_system_obsolete =
        (unit_system_from_config != this->model->unit_system);
    if (is_unit_system_obsolete) {
      this->model->unit_system = unit_system_from_config;
    }

    const auto town_from_config =
        config.read_string("location_town"s, "Paris"s);
    const auto country_from_config =
        config.read_string("location_country"s, "France"s);

    const bool is_town_or_country_obsolete =
        (town_from_config != this->model->location.town) or
        (country_from_config != this->model->location.country);

    if (is_town_or_country_obsolete) {
      this->model->location.town = town_from_config;
      this->model->location.country = country_from_config;
      this->clear_model_weather_conditions();
    }

    const auto display_daily_forecast_from_config =
        config.read_bool("display_daily_forecast"s, false);
    const bool is_display_daily_forecast_obsolete =
        (display_daily_forecast_from_config !=
         this->model->display_daily_forecast);
    if (is_display_daily_forecast_obsolete) {
      this->model->display_daily_forecast = display_daily_forecast_from_config;
    }

    const std::string current_system_language = currentLang();
    const bool is_language_obsolete =
        (this->language != current_system_language);
    if (is_language_obsolete) {
      this->language = current_system_language;
      initialize_translations();
    }

    const bool is_data_obsolete =
        is_api_key_obsolete or is_unit_system_obsolete or
        is_town_or_country_obsolete or is_language_obsolete;
    // temperatures, wind speed and weather description are computed
    // by the backend thus unit system or language change implies that
    // data are obsolete

    const auto event_handler = GetEventHandler();
    if (is_data_obsolete) {
      SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data, 0);
    } else if (is_display_daily_forecast_obsolete) {
      SendEvent(event_handler, EVT_CUSTOM,
                CustomEvent::model_daily_forecast_display_changed, 0);
    }
  }

  int handle_custom_event(int param_one, int param_two) {
    // Commands
    if (param_one == CustomEvent::change_daily_forecast_display) {
      const bool enable = not this->model->display_daily_forecast;
      this->display_daily_forecast(enable);
    } else if (param_one == CustomEvent::change_location) {
      auto *const raw_location =
          reinterpret_cast<std::array<char, 256> *>(GetCurrentEventExData());
      const std::string location{raw_location->data()};
      this->update_config_location(location);
      return 1;
    } else if (param_one == CustomEvent::change_unit_system) {
      this->change_unit_system(static_cast<UnitSystem>(param_two));
    } else if (param_one == CustomEvent::display_alert) {
      if (this->ui) {
        this->ui->display_alert();
        return 1;
      }
    } else if (param_one == CustomEvent::refresh_data) {
      this->refresh_data();
      return 1;
    } else if (param_one == CustomEvent::select_location_from_history) {
      const size_t history_index = param_two;
      auto location = this->history->get_location(history_index);
      if (location) {
        this->update_config_location(*location);
      } else {
        DialogSynchro(ICON_WARNING, "Title", "Location not found!",
                      GetLangText("Ok"), nullptr, nullptr);
      }
      return 1;
    } else if (param_one == CustomEvent::show_about_dialog) {
      this->open_about_dialog();
      return 1;
    } else if (param_one == CustomEvent::toggle_current_location_favorite) {
      this->history->toggle_current_location_favorite();
      return 1;
    }

    // Events
    if (param_one == CustomEvent::model_daily_forecast_display_changed) {
      if (this->ui) {
        ui->switch_forecast_widget();
      }
      return 1;
    } else if (param_one == CustomEvent::model_updated) {
      this->model->refresh_date = std::time(nullptr);
      this->history->update_history_maybe();
      this->show();
    } else if (param_one == CustomEvent::warning_emitted) {
      if (param_two == CustomEventParam::invalid_location) {
        DialogSynchro(
            ICON_WARNING, GetLangText("Invalid input"),
            GetLangText(
                "Make sure you enter a town then a country separated by a "
                "comma."),
            GetLangText("Ok"), nullptr, nullptr);
        return 1;
      }
    }
    return 0;
  }

  void clear_model_weather_conditions() {
    this->model->current_condition = std::experimental::nullopt;
    this->model->hourly_forecast.clear();
    this->model->daily_forecast.clear();
    this->model->refresh_date = 0;
  }

  void refresh_data() {
    ShowHourglassForce();

    ClearTimerByName(App::refresh_timer_name);

    this->model->refresh_date = 0;

    const auto units = Units{this->model}.to_string();
    try {
      this->service->fetch_data(this->model->location.town,
                                this->model->location.country, currentLang(),
                                units);

      this->model->current_condition = this->service->get_current_condition();

      this->model->hourly_forecast.clear();
      for (auto &forecast : this->service->get_hourly_forecast()) {
        this->model->hourly_forecast.push_back(forecast);
      }

      this->model->daily_forecast.clear();
      for (auto &forecast : this->service->get_daily_forecast()) {
        this->model->daily_forecast.push_back(forecast);
      }

      this->model->alerts = this->service->get_alerts();

      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM, CustomEvent::model_updated, 0);

    } catch (const ConnectionError &error) {
      Message(
          ICON_WARNING, GetLangText("Network error"),
          GetLangText("Failure while fetching weather data. Check your network "
                      "connection."),
          App::error_dialog_delay);
    } catch (const RequestError &error) {
      Message(
          ICON_WARNING, GetLangText("Network error"),
          GetLangText("Failure while fetching weather data. Check your network "
                      "connection."),
          App::error_dialog_delay);
    } catch (const ServiceError &error) {
      Message(ICON_WARNING, GetLangText("Service unavailable"), error.what(),
              App::error_dialog_delay);
    }
    SetHardTimer(App::refresh_timer_name, &taranis::App::refresh_data_maybe,
                 App::refresh_period);
    HideHourglass();
  }

  void open_about_dialog() {
    const auto about_content = get_about_content();
    Dialog(ICON_INFORMATION, GetLangText("About"), about_content.c_str(),
           GetLangText("Ok"), nullptr, &handle_about_dialog_button_clicked);
  }

  static Location parse_location(const std::string &location) {
    std::stringstream to_parse{location};
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(to_parse, token, ',')) {
      ltrim(token);
      rtrim(token);
      tokens.push_back(token);
    }
    if (tokens.size() == 1) {
      auto town = tokens[0];
      town[0] = std::toupper(town[0]);
      return {town, ""};
    } else if (tokens.size() > 1) {
      auto country = tokens[tokens.size() - 1];
      country[0] = std::toupper(country[0]);

      auto town = location.substr(0, location.size() - country.size());
      ltrim(town);
      rtrim(town);
      town[0] = std::toupper(town[0]);

      return {town, country};
    }
    throw InvalidLocation{};
  }

  void update_config_location(const std::string &text) const {
    if (text == format_location(this->model->location)) {
      return;
    }
    try {
      auto location = App::parse_location(text);

      this->update_config_location(location);
    } catch (const InvalidLocation &error) {
      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM, CustomEvent::warning_emitted,
                CustomEventParam::invalid_location);
    }
  }

  void update_config_location(const Location &location) const {
    Config config;
    config.write_string("location_town"s, location.town);
    config.write_string("location_country"s, location.country);

    Config::config_changed();
  }

  void change_unit_system(UnitSystem unit_system) {
    if (unit_system == this->model->unit_system) {
      return;
    }

    Config config;
    config.write_int("unit_system", unit_system);

    Config::config_changed();
  }

  void display_daily_forecast(bool enable) const {
    if (enable == this->model->display_daily_forecast) {
      return;
    }
    Config config;
    config.write_bool("display_daily_forecast", enable);

    Config::config_changed();
  }

  static void refresh_data_maybe();
};
} // namespace taranis
