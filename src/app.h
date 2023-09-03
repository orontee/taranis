#pragma once

#include <array>
#include <cctype>
#include <cstring>
#include <ctime>
#include <inkview.h>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "config.h"
#include "errors.h"
#include "events.h"
#include "l10n.h"
#include "model.h"
#include "service.h"
#include "ui.h"
#include "util.h"

#define T(x) GetLangText(x)

using namespace std::placeholders;
using namespace std::string_literals;

namespace taranis {

void handle_about_dialog_button_clicked(int button_index);

std::string get_about_content();

class App {
public:
  App()
      : model{std::make_shared<Model>()}, service{std::make_unique<Service>()} {
  }

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
      this->clear_model_weather_conditions();
      this->show();
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
  static const int error_dialog_delay{3600};

  std::shared_ptr<Model> model;
  std::unique_ptr<Service> service;
  std::unique_ptr<Ui> ui;

  void setup() { this->ui = std::make_unique<Ui>(this->model); }

  void show() {
    if (not this->ui)
      return;

    this->load_config();
    this->ui->show();
    this->refresh_model_weather_conditions();
  }

  void exit() {
    this->ui.reset();
    this->service.reset();
    this->model.reset();

    CloseApp();
  }

  void load_config() {
    Config config;

    this->model->location.town = config.read_string("location_town"s, "Paris"s);

    this->model->location.country =
        config.read_string("location_country"s, "France"s);

    this->service->set_api_key(config.read_string(
        "openweather_api_key"s, "4620ad6f20069b66bc36b1e88ceb4541"s));
    // API key associated to rate limited plan

    this->model->source = "OpenWeather"s;

    initialize_translations();
  }

  int handle_custom_event(int param_one, int param_two) {
    if (param_one == CustomEvent::config_editor_requested) {
      this->open_config_editor();
      return 1;
    } else if (param_one == CustomEvent::about_dialog_requested) {
      this->open_about_dialog();
      return 1;
    } else if (param_one == CustomEvent::model_updated) {
      if (this->ui) {
        this->ui->show();
        return 1;
      }
    } else if (param_one == CustomEvent::new_location_requested) {
      auto *const raw_location =
          reinterpret_cast<std::array<char, 256> *>(GetCurrentEventExData());
      const std::string location{raw_location->data()};
      update_config_location(location);
      return 1;
    } else if (param_one == CustomEvent::refresh_requested) {
      this->refresh_model_weather_conditions();
      return 1;
    } else if (param_one == CustomEvent::warning_emitted) {
      if (param_two == CustomEventParam::invalid_location) {
        DialogSynchro(
            ICON_WARNING, T("Invalid input"),
            T("Make sure you enter a town then a country separated by a "
              "comma."),
            T("Ok"), nullptr, nullptr);
        return 1;
      }
    }
    return 0;
  }

  void clear_model_weather_conditions() {
    this->model->current_condition = std::optional<Condition>{};
    this->model->hourly_forecast.clear();
    this->model->refresh_date = 0;
  }

  void refresh_model_weather_conditions() {
    ShowHourglassForce();

    this->model->refresh_date = std::time(nullptr);

    try {
      std::vector<Condition> conditions = this->service->fetch_conditions(
          this->model->location.town, this->model->location.country,
          currentLang());

      this->model->current_condition = conditions.front();

      this->model->hourly_forecast.clear();

      auto it = std::begin(conditions);
      ++it; // skip current
      while (it != std::end(conditions)) {
        this->model->hourly_forecast.push_back(*it);
        ++it;
      }

      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM, CustomEvent::model_updated, 0);
    } catch (const ConnectionError &error) {
      Message(ICON_WARNING, T("Network error"),
              T("Failure while fetching weather data. Check your network "
                "connection."),
              App::error_dialog_delay);
    } catch (const RequestError &error) {
      Message(ICON_WARNING, T("Network error"),
              T("Failure while fetching weather data. Check your network "
                "connection."),
              App::error_dialog_delay);
    } catch (const ServiceError &error) {
      Message(ICON_WARNING, T("Service unavailable"), error.what(),
              App::error_dialog_delay);
    }
    HideHourglass();
  }

  void open_config_editor() {
    Config config;
    config.open_editor();
  }

  void open_about_dialog() {
    const auto about_content = get_about_content();
    Dialog(ICON_INFORMATION, T("About"), about_content.c_str(), T("Ok"),
           nullptr, &handle_about_dialog_button_clicked);
  }

  static std::pair<std::string, std::string>
  extract_town_and_country(const std::string &location) {
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
      return {tokens[0], ""};
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

  void update_config_location(const std::string &location) {
    try {
      auto [town, country] = App::extract_town_and_country(location);

      Config config;
      config.write_string("location_town"s, town);
      config.write_string("location_country"s, country);

      Config::config_changed();
    } catch (const InvalidLocation &error) {
      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM, CustomEvent::warning_emitted,
                CustomEventParam::invalid_location);
    }
  }
};

} // namespace taranis
