#include "app.h"

#include <experimental/optional>
#include <inkview.h>

#include "config.h"
#include "errors.h"
#include "events.h"
#include "model.h"
#include "openweather.h"
#include "state.h"
#include "units.h"
#include "util.h"

namespace taranis {
App::App()
    : client{std::make_shared<HttpClient>()}, model{std::make_shared<Model>()},
      config{std::make_shared<Config>()}, l10n{std::make_unique<L10n>()},
      service{std::make_unique<OpenWeatherService>(this->client)},
      version_checker{std::make_unique<VersionChecker>(
          this->config, this->client, this->model)},
      history{std::make_unique<LocationHistoryProxy>(this->model)},
      application_state{std::make_unique<ApplicationState>(this->model)},
      task_icon{BitmapStretchCopy(
          &icon_taranis, (icon_taranis.width - task_icon_size) / 2,
          (icon_taranis.height - task_icon_size) / 2, task_icon_size,
          task_icon_size, task_icon_size, task_icon_size)} {}

int App::process_event(int event_type, int param_one, int param_two) {
  if (event_type != EVT_MTSYNC) {
    // No need to log multi touch synchronization events

    BOOST_LOG_TRIVIAL(debug)
        << "Processing event of type " << format_event_type(event_type);
  }
  if (event_type == EVT_INIT) {
    this->setup();
    return 1;
  }

  if (event_type == EVT_SHOW or
      (event_type == EVT_ACTIVATE and this->show_on_next_activate)) {
    this->show();
    this->show_on_next_activate = false;
    return 1;
  }

  if (event_type == EVT_HIDE) {
    this->show_on_next_activate = true;

    // An event of type EVT_HIDE is received before the config editor
    // and location selector are opened; An event of type EVT_ACTIVATE
    // is received when they're closed. The boolean
    // `show_on_next_activate' is used to not paint the application
    // interface on each event of type EVT_ACTIVATE (such events are
    // received when menu, keyboard, etc. are closed). See #73.

    return 1;
  }

  if (event_type == EVT_EXIT) {
    this->exit();
    return 0;
  }

  if (event_type == EVT_CONFIGCHANGED) {
    this->load_config();
    return 1;
  }

  if (event_type == EVT_CUSTOM) {
    return this->handle_custom_event(param_one, param_two);
  }

  if (this->ui) {
    if (ISPOINTEREVENT(event_type) || event_type == EVT_SCROLL) {
      return this->ui->handle_pointer_event(event_type, param_one, param_two);
    }
    if (ISKEYEVENT(event_type)) {
      return this->ui->handle_key_event(event_type, param_one);
    }
    if (event_type == EVT_SCREEN_INVERSION_MODE_CHANGED) {
      this->ui->generate_logo_maybe();
    }
  }
  return 0;
}

void App::setup() {
  BOOST_LOG_TRIVIAL(info) << "Application setup";

  this->l10n->initialize_translations();
  this->check_minimum_supported_firmware();
  this->set_app_capabilities();
  this->set_task_parameters();

  this->application_state->restore();

  auto &current_location = this->model->location;
  if (current_location.name.empty()) {
    BOOST_LOG_TRIVIAL(debug) << "Setting current location to default value "
                             << "since not restored from application state";
    current_location.longitude = 2.3200410217200766;
    current_location.latitude = 48.858889699999999;
    current_location.name = "Paris";
    current_location.country = "FR";
    current_location.state = "Ile-de-France";
  } else {
    BOOST_LOG_TRIVIAL(debug)
        << "Current location restored from application state";
  }
  this->load_config();

  this->ui = std::make_unique<Ui>(this->config, this->model);
}

void App::show() {
  if (not this->ui)
    return;

  this->ui->paint();
}

void App::exit() {
  BOOST_LOG_TRIVIAL(info) << "Exiting application";

  if (this->application_state != nullptr) {
    this->application_state->dump();
  }

  this->ui.reset();
  this->application_state.reset();
  this->history.reset();
  this->version_checker.reset();
  this->service.reset();
  this->model.reset();
  this->client.reset();
}

void App::load_config() {
  if (not this->config_already_loaded) {
    BOOST_LOG_TRIVIAL(debug) << "Loading configuration";
  } else {
    BOOST_LOG_TRIVIAL(debug) << "Rereading configuration";
  }

  const auto api_key_from_config = this->config->read_string("api_key", "");
  const auto is_api_key_obsolete =
      (not api_key_from_config.empty() and
       api_key_from_config != this->service->get_api_key());
  if (is_api_key_obsolete) {
    this->service->set_api_key(api_key_from_config);
  }

  const auto unit_system_from_config = static_cast<UnitSystem>(
      this->config->read_int("unit_system"s, UnitSystem::metric));
  const bool is_unit_system_obsolete =
      (unit_system_from_config != this->model->unit_system);
  if (is_unit_system_obsolete) {
    this->model->unit_system = unit_system_from_config;
  }

  if (not this->config_already_loaded) {
    const auto start_with_daily_forecast_from_config =
        this->config->read_bool("start_with_daily_forecast"s, false);
    this->model->display_daily_forecast = start_with_daily_forecast_from_config;
  }

  const auto data_update_strategy_from_config =
      static_cast<DataUpdateStrategy>(this->config->read_int(
          "data_update_strategy"s, DataUpdateStrategy::hourly_from_startup));
  const bool is_data_update_strategy_obsolete =
      (data_update_strategy_from_config != this->model->data_update_strategy);
  if (is_data_update_strategy_obsolete) {
    this->model->data_update_strategy = data_update_strategy_from_config;
  }

  const bool is_language_obsolete = this->l10n->is_language_obsolete();
  if (is_language_obsolete) {
    this->l10n->update_translations();
  }

  const bool is_data_obsolete = not this->can_keep_data_at_startup() or
                                is_unit_system_obsolete or is_language_obsolete;
  // temperatures, wind speed and weather description are computed
  // by the backend thus unit system or language change implies that
  // data are obsolete

  if (this->ui) {
    this->ui->generate_logo_maybe();

    // A logo generation must be requested after the config changed
    // since no model update (which triggers a logo generation) may
    // happen before user open the device configuration panel for
    // logos (See issue #28)

    // Also don't bother for the value of the configuration key
    // generate_shutdown_logo since if it changed to false, one must
    // remove the logo (handled by generate_logo_maybe(), yeah not
    // that intuitive...)
  }

  const auto event_handler = GetEventHandler();
  if (is_data_obsolete) {
    const auto context = this->config_already_loaded
                             ? CallContext::triggered_by_configuration_change
                             : CallContext::triggered_by_application_startup;

    BOOST_LOG_TRIVIAL(debug)
        << "Data is considered obsolete because of strategy: "
        << not this->can_keep_data_at_startup()
        << ", of obsolete unit system: " << is_unit_system_obsolete
        << ", of obsolete_language: " << is_language_obsolete
        << " and will be refreshed with context " << context;

    SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data, context);
  }
  this->config_already_loaded = true;
}

int App::handle_custom_event(int param_one, int param_two) {
  BOOST_LOG_TRIVIAL(debug) << "Handling custom event "
                           << format_custom_event_param(param_one) << " "
                           << param_two;

  // Commands
  if (param_one == CustomEvent::change_daily_forecast_display) {
    this->model->display_daily_forecast =
        not this->model->display_daily_forecast;

    const auto event_handler = GetEventHandler();
    SendEvent(event_handler, EVT_CUSTOM,
              CustomEvent::model_daily_forecast_display_changed, 0);
    return 1;
  } else if (param_one == CustomEvent::check_application_version) {
    this->version_checker->check(CallContext::triggered_by_user);
    return 1;
  } else if (param_one == CustomEvent::search_location) {
    auto *const raw_location =
        reinterpret_cast<std::array<char, 256> *>(GetCurrentEventExData());
    const std::string location{raw_location->data()};
    this->search_location(location);
    return 1;
  } else if (param_one == CustomEvent::edit_location) {
    if (this->ui) {
      this->ui->edit_location();
      return 1;
    }
  } else if (param_one == CustomEvent::open_alert_viewer) {
    if (this->ui) {
      this->ui->display_alert();
      return 1;
    }
  } else if (param_one == CustomEvent::open_config_editor) {
    this->config->open_editor();
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
  } else if (param_one == CustomEvent::open_about_dialog) {
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
    this->model->refresh_date = std::chrono::system_clock::now();
    this->history->update_history_maybe();
    this->show();

    if (this->ui) {
      this->ui->generate_logo_maybe();
    }

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
  this->model->refresh_date = std::experimental::nullopt;
}

bool App::can_keep_data_at_startup() const {
  if (this->config_already_loaded) {
    return true;
  }

  if (this->model->data_update_strategy ==
      DataUpdateStrategy::hourly_from_startup) {
    BOOST_LOG_TRIVIAL(debug) << "Can't keep data due to data update strategy";
    return false;
  }

  if (this->model->data_update_strategy ==
      DataUpdateStrategy::hourly_when_obsolete) {
    if (this->model->refresh_date == std::experimental::nullopt) {
      return false;
    }
    BOOST_LOG_TRIVIAL(debug)
        << "Will check whether refresh delay is exhausted or not";
    if (delay_exhausted_from(*(this->model->refresh_date),
                             App::refresh_period)) {
      BOOST_LOG_TRIVIAL(debug)
          << "Can't keep data considered obsolete for data update strategy";
      return false;
    }
  }
  if (this->is_data_refresh_periodic()) {
    this->start_refresh_timer();
  }
  return true;
}

bool App::must_skip_data_refresh(CallContext context) const {
  const bool should_respect_flight_mode_enabled =
      (context == CallContext::triggered_by_application_startup or
       context == CallContext::triggered_by_timer or
       context == CallContext::triggered_by_user);
  if (should_respect_flight_mode_enabled and IsFlightModeEnabled()) {
    BOOST_LOG_TRIVIAL(info)
        << "Won't refresh data since device has flight mode enabled";
    return true;
  }

  const bool should_respect_not_connected =
      (context == CallContext::triggered_by_timer);
  if (should_respect_not_connected) {
    iv_netinfo *netinfo = NetInfo();
    if (netinfo == nullptr or not netinfo->connected) {
      BOOST_LOG_TRIVIAL(debug) << "Won't refresh data since not connected";
      return true;
    }
  }
  return false;
}

bool App::is_data_refresh_periodic() const {
  return (this->model->data_update_strategy ==
              DataUpdateStrategy::hourly_from_startup or
          this->model->data_update_strategy ==
              DataUpdateStrategy::hourly_when_obsolete);
}

void App::refresh_data(CallContext context) {
  BOOST_LOG_TRIVIAL(debug) << "Refreshing data";

  this->cancel_refresh_timer();

  const bool force =
      (context == CallContext::triggered_by_configuration_change or
       context == CallContext::triggered_by_model_change);

  if (this->must_skip_data_refresh(context)) {
    if (this->is_data_refresh_periodic()) {
      this->start_refresh_timer();
    }
    return;
  }
  ShowHourglassForce();

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

  if (this->is_data_refresh_periodic()) {
    this->start_refresh_timer();
  }
  HideHourglass();
}

void App::open_about_dialog() {
  BOOST_LOG_TRIVIAL(debug) << "Opening about dialog";

  const auto about_content = get_about_content();
  DialogSynchro(ICON_INFORMATION, GetLangText("Software version"),
                about_content.c_str(), const_cast<char *>(GetLangText("Ok")),
                nullptr, nullptr);
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
        << "Connection error while searching locations " << error.what();
    Message(
        ICON_WARNING, GetLangText("Network error"),
        GetLangText("Failure while fetching weather data. Check your network "
                    "connection."),
        App::error_dialog_delay);
  } catch (const RequestError &error) {
    BOOST_LOG_TRIVIAL(debug)
        << "Request error while searching locations " << error.what();
    Message(
        ICON_WARNING, GetLangText("Network error"),
        GetLangText("Failure while fetching weather data. Check your network "
                    "connection."),
        App::error_dialog_delay);
  } catch (const ServiceError &error) {
    BOOST_LOG_TRIVIAL(debug)
        << "Service error while searching locations " << error.what();
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

void App::check_minimum_supported_firmware() {
  const auto firmware_version = GetSoftwareVersion();
  try {
    firmware_version_greater_than(firmware_version, 6);
  } catch (const UnsupportedFirmwareVersion &error) {
    BOOST_LOG_TRIVIAL(warning)
        << "Unsupported firmware version, " << firmware_version;
    Message(ICON_WARNING, GetLangText("Unsupported software version"),
            GetLangText("The application isn't compatible with the software "
                        "version of this reader."),
            error_dialog_delay);
    this->exit();
    return;
  }
}

void App::set_app_capabilities() {
  const auto firmware_version = GetSoftwareVersion();
  try {
    firmware_version_greater_than(firmware_version, 6, 8);
  } catch (const UnsupportedFirmwareVersion &error) {
    BOOST_LOG_TRIVIAL(warning)
        << "Firmware version not supporting dark mode, " << firmware_version;
    return;
  }
  IvSetAppCapability(APP_CAPABILITY_SUPPORT_SCREEN_INVERSION);
}

void App::set_task_parameters() {
  const auto task_identifier = GetCurrentTask();
  const auto task_info = GetTaskInfo(task_identifier);
  if (task_info) {
    BOOST_LOG_TRIVIAL(debug) << "Setting task parameters";

    SetTaskParameters(task_identifier, task_info->appname, "Taranis",
                      this->task_icon.get(), task_info->flags);
  } else {
    BOOST_LOG_TRIVIAL(warning) << "No task info!";
  }
}

void App::start_refresh_timer() const {
  SetHardTimer(App::refresh_timer_name, &taranis::App::refresh_timer_callback,
               App::refresh_period);
}

void App::cancel_refresh_timer() const {
  ClearTimerByName(App::refresh_timer_name);
}

void App::refresh_timer_callback() {
  BOOST_LOG_TRIVIAL(debug) << "Data refresh requested by timer";

  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_CUSTOM, CustomEvent::refresh_data,
            CallContext::triggered_by_timer);
}
} // namespace taranis
