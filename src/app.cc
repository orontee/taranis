#include "app.h"

#include "config.h"
#include "errors.h"
#include "events.h"
#include "inkview.h"
#include "model.h"
#include "units.h"
#include "util.h"

namespace taranis {

bool config_already_loaded{false};

int App::process_event(int event_type, int param_one, int param_two) {
  BOOST_LOG_TRIVIAL(debug) << "Processing event of type "
                           << format_event_type(event_type);

  if (event_type == EVT_INIT) {
    this->setup();
    return 1;
  }

  if (event_type == EVT_SHOW or event_type == EVT_FOREGROUND) {
    this->show();
    return 1;
  }

  if (event_type == EVT_HIDE or event_type == EVT_BACKGROUND) {
    return 1;
  }

  if (event_type == EVT_EXIT) {
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

void App::setup() {
  BOOST_LOG_TRIVIAL(info) << "Application setup";

  this->l10n->initialize_translations();

  const auto version = GetSoftwareVersion();
  try {
    check_software_version(version);
  } catch (const UnsupportedSoftwareVersion &error) {
    BOOST_LOG_TRIVIAL(warning) << "Unsupported software version " << version;
    Message(ICON_WARNING, GetLangText("Unsupported software version"),
            GetLangText("The application isn't compatible with the software "
                        "version of this reader."),
            error_dialog_delay);
    this->exit();
    return;
  }
  this->ui = std::make_unique<Ui>(this->model);

  auto &current_location = this->model->location;
  if (current_location.name.empty()) {
    current_location.longitude = 2.3200410217200766;
    current_location.latitude = 48.858889699999999;
    current_location.name = "Paris";
    current_location.country = "FR";
    current_location.state = "Ile-de-France";
  }
  this->load_config();
}

void App::show() {
  if (not this->ui)
    return;

  this->ui->show();
}

void App::exit() {
  BOOST_LOG_TRIVIAL(info) << "Exiting application";

  this->ui.reset();
  this->history.reset();
  this->application_state.reset();
  this->service.reset();
  this->model.reset();

  CloseApp();
}

void App::load_config() {
  if (not config_already_loaded) {
    BOOST_LOG_TRIVIAL(debug) << "Loading configuration";
  } else {
    BOOST_LOG_TRIVIAL(debug) << "Rereading configuration";
  }

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

  const auto display_daily_forecast_from_config =
      config.read_bool("display_daily_forecast"s, false);
  const bool is_display_daily_forecast_obsolete =
      (display_daily_forecast_from_config !=
       this->model->display_daily_forecast);
  if (is_display_daily_forecast_obsolete) {
    this->model->display_daily_forecast = display_daily_forecast_from_config;
  }

  const std::string current_system_language = currentLang();
  const bool is_language_obsolete = this->l10n->is_language_obsolete();
  if (is_language_obsolete) {
    this->l10n->update_translations();
  }

  const bool is_data_obsolete = not config_already_loaded or
                                is_api_key_obsolete or
                                is_unit_system_obsolete or is_language_obsolete;
  // temperatures, wind speed and weather description are computed
  // by the backend thus unit system or language change implies that
  // data are obsolete

  const auto event_handler = GetEventHandler();
  if (is_data_obsolete) {
    const auto context = config_already_loaded
                             ? CallContext::triggered_by_configuration_change
                             : CallContext::triggered_by_application_startup;
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data, context);
  } else if (is_display_daily_forecast_obsolete) {
    SendEvent(event_handler, EVT_CUSTOM,
              CustomEvent::model_daily_forecast_display_changed, 0);
  }
  config_already_loaded = true;
}

int App::handle_custom_event(int param_one, int param_two) {
  BOOST_LOG_TRIVIAL(debug) << "Handling custom event "
                           << format_custom_event_param(param_one) << " "
                           << param_two;

  // Commands
  if (param_one == CustomEvent::change_daily_forecast_display) {
    const bool enable = not this->model->display_daily_forecast;
    this->update_configured_display_daily_forecast(enable);
  } else if (param_one == CustomEvent::search_location) {
    auto *const raw_location =
        reinterpret_cast<std::array<char, 256> *>(GetCurrentEventExData());
    const std::string location{raw_location->data()};
    this->search_location(location);
    return 1;
  } else if (param_one == CustomEvent::change_unit_system) {
    this->update_configured_unit_system(static_cast<UnitSystem>(param_two));
  } else if (param_one == CustomEvent::display_alert) {
    if (this->ui) {
      this->ui->display_alert();
      return 1;
    }
  } else if (param_one == CustomEvent::refresh_data) {
    this->refresh_data(static_cast<CallContext>(param_two));
    return 1;
  } else if (param_one == CustomEvent::select_location_from_history) {
    const size_t history_index = param_two;
    const auto location = this->history->get_location(history_index);
    if (location) {
      this->set_model_location(*location);
    } else {
      DialogSynchro(ICON_WARNING, "Title", "Location not found!",
                    GetLangText("Ok"), nullptr, nullptr);
    }
    return 1;
  } else if (param_one == CustomEvent::select_location_from_list) {
    const size_t list_index = param_two;
    if (this->ui) {
      const auto location =
          this->ui->get_location_from_location_list(list_index);
      if (location) {
        this->set_model_location(*location);
      } else {
        DialogSynchro(ICON_WARNING, "Title", "Location not found!",
                      GetLangText("Ok"), nullptr, nullptr);
      }
      return 1;
    }
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

void App::clear_model_weather_conditions() {
  BOOST_LOG_TRIVIAL(debug) << "Clearing weather conditions stored in model";

  this->model->current_condition = std::experimental::nullopt;
  this->model->hourly_forecast.clear();
  this->model->daily_forecast.clear();
  this->model->alerts.clear();
  this->model->refresh_date = 0;
}

bool App::must_skip_data_refresh() const {
  if (IsFlightModeEnabled()) {
    BOOST_LOG_TRIVIAL(info)
        << "Won't refresh data since device has flight mode enabled";
    return true;
  }
  return false;
}

void App::refresh_data(CallContext context) {
  BOOST_LOG_TRIVIAL(debug) << "Refreshing data";

  const bool force =
      (context == CallContext::triggered_by_configuration_change or
       context == CallContext::triggered_by_model_change);
  // At application startup, when refresh timer triggers, or when user
  // triggered the refresh, one must respect flight mode being enabled

  if (not force and this->must_skip_data_refresh()) {
    return;
  }
  ShowHourglassForce();

  ClearTimerByName(App::refresh_timer_name);

  const auto units = Units{this->model}.to_string();
  bool failed = false;
  Condition current_condition;
  std::vector<Condition> hourly_forecast;
  std::vector<DailyCondition> daily_forecast;
  std::vector<Alert> alerts;
  this->service->set_location(this->model->location);
  try {
    this->service->fetch_data(currentLang(), units);

    current_condition = this->service->get_current_condition();
    hourly_forecast = this->service->get_hourly_forecast();
    daily_forecast = this->service->get_daily_forecast();
    alerts = this->service->get_alerts();

  } catch (const ConnectionError &error) {
    failed = true;
    BOOST_LOG_TRIVIAL(debug)
        << "Connection error while refreshing weather conditions "
        << error.what();
    Message(
        ICON_WARNING, GetLangText("Network error"),
        GetLangText("Failure while fetching weather data. Check your network "
                    "connection."),
        App::error_dialog_delay);
  } catch (const RequestError &error) {
    failed = true;
    BOOST_LOG_TRIVIAL(debug)
        << "Request error while refreshing weather conditions " << error.what();
    Message(
        ICON_WARNING, GetLangText("Network error"),
        GetLangText("Failure while fetching weather data. Check your network "
                    "connection."),
        App::error_dialog_delay);
  } catch (const ServiceError &error) {
    failed = true;
    BOOST_LOG_TRIVIAL(debug)
        << "Service error while refreshing weather conditions " << error.what();
    Message(ICON_WARNING, GetLangText("Service unavailable"), error.what(),
            App::error_dialog_delay);
  }
  if (not failed or force) {
    clear_model_weather_conditions();

    // if model isn't cleared and a forced refresh has been required
    // (user changes language or location), incoherent data will be
    // shown!
  }
  if (not failed) {
    this->model->current_condition = current_condition;
    this->model->hourly_forecast = hourly_forecast;
    this->model->daily_forecast = daily_forecast;
    this->model->alerts = alerts;

    const auto event_handler = GetEventHandler();
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::model_updated, 0);
  } else {
    if (force) {
      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM, CustomEvent::model_updated, 0);
    } else {
      this->show();
    }
  }
  // Make sure to redraw screen otherwise display could be incoherent:
  // Parts of the screen may have been restored to their state before
  // a dialog on device not being connected popup, which doesn't
  // respect the reset implemented in case of a forced refresh…

  SetHardTimer(App::refresh_timer_name, &taranis::App::refresh_data_maybe,
               App::refresh_period);

  HideHourglass();
}

void App::open_about_dialog() {
  BOOST_LOG_TRIVIAL(debug) << "Opening about dialog";

  const auto about_content = get_about_content();
  Dialog(ICON_INFORMATION, GetLangText("About"), about_content.c_str(),
         GetLangText("Ok"), nullptr, &handle_about_dialog_button_clicked);
}

void App::search_location(const std::string &location_description) {
  try {
    const auto [town, country] =
        parse_location_description(location_description);
    const auto found_locations = service->search_location(town, country);

    if (found_locations.size() == 1) {
      this->set_model_location(found_locations[0]);
    } else if (found_locations.size() > 1) {
      if (this->ui) {
        this->ui->open_location_list(found_locations);
      }
    }
  } catch (const InvalidLocation &error) {
    const auto event_handler = GetEventHandler();
    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::warning_emitted,
              CustomEventParam::invalid_location);
  } catch (const ConnectionError &error) {
    BOOST_LOG_TRIVIAL(debug)
        << "Connection error while refreshing weather conditions "
        << error.what();
    Message(
        ICON_WARNING, GetLangText("Network error"),
        GetLangText("Failure while fetching weather data. Check your network "
                    "connection."),
        App::error_dialog_delay);
  } catch (const RequestError &error) {
    BOOST_LOG_TRIVIAL(debug)
        << "Request error while refreshing weather conditions " << error.what();
    Message(
        ICON_WARNING, GetLangText("Network error"),
        GetLangText("Failure while fetching weather data. Check your network "
                    "connection."),
        App::error_dialog_delay);
  } catch (const ServiceError &error) {
    BOOST_LOG_TRIVIAL(debug)
        << "Service error while refreshing weather conditions " << error.what();
    Message(ICON_WARNING, GetLangText("Service unavailable"), error.what(),
            App::error_dialog_delay);
  }
}

void App::set_model_location(const Location &location) const {
  BOOST_LOG_TRIVIAL(debug) << "Updating model location";

  if (location == this->model->location) {
    BOOST_LOG_TRIVIAL(debug) << "No need to update model location";

    return;
  }
  this->model->location = location;

  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data,
            CallContext::triggered_by_model_change);
}

void App::update_configured_unit_system(UnitSystem unit_system) {
  BOOST_LOG_TRIVIAL(debug) << "Updating configured unit system";

  if (unit_system == this->model->unit_system) {
    BOOST_LOG_TRIVIAL(debug) << "No need to update configured unit system";

    return;
  }

  Config config;
  config.write_int("unit_system", unit_system);

  Config::config_changed();
}

void App::update_configured_display_daily_forecast(bool enable) const {
  BOOST_LOG_TRIVIAL(debug) << "Updating configured display daily forecast";

  if (enable == this->model->display_daily_forecast) {
    BOOST_LOG_TRIVIAL(debug)
        << "No need to update configured display daily forecast";
    return;
  }
  Config config;
  config.write_bool("display_daily_forecast", enable);

  Config::config_changed();
}

void App::refresh_data_maybe() {
  BOOST_LOG_TRIVIAL(debug) << "Refreshing data maybe";

  iv_netinfo *netinfo = NetInfo();
  if (netinfo == nullptr or not netinfo->connected) {
    SetHardTimer(App::refresh_timer_name, &taranis::App::refresh_data_maybe,
                 App::refresh_period);

    BOOST_LOG_TRIVIAL(debug) << "Restarting refresh timer since not connected";
    return;
  }
  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data,
            CallContext::triggered_by_timer);
}

void App::handle_about_dialog_button_clicked(int button_index) {
  CloseDialog();
}
} // namespace taranis
