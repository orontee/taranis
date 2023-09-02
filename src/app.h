#pragma once

#include <array>
#include <cstring>
#include <ctime>
#include <inkview.h>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>

#include "config.h"
#include "errors.h"
#include "events.h"
#include "l10n.h"
#include "model.h"
#include "service.h"
#include "ui.h"

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
      this->load_config();
      if (this->ui) {
        this->ui->show();
      }
      return 1;
    }

    if (event_type == EVT_CUSTOM) {
      return this->handle_custom_event(param_one, param_two);
    }

    if (this->ui) {
      if (event_type == EVT_POINTERDOWN || event_type == EVT_POINTERDRAG ||
          event_type == EVT_POINTERUP) {
        return this->ui->handle_pointer_event(event_type, param_one, param_two);
      }
      if (event_type == EVT_KEYPRESS) {
        return this->ui->handle_key_pressed(param_one);
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
    this->refresh_model();
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
    // API key associated to open-source plan

    this->model->source = "OpenWeather";

    initialize_translations();
  }

  int handle_custom_event(int param_one, int param_two) {
    if (param_one == CustomEvent::model_updated) {
      if (this->ui) {
        this->ui->show();
        return 1;
      }
    } else if (param_one == CustomEvent::refresh_requested) {
      this->refresh_model();
      return 1;

    } else if (param_one == CustomEvent::config_editor_requested) {
      this->open_config_editor();
    } else if (param_one == CustomEvent::about_dialog_requested) {
      this->open_about_dialog();
      return 1;
    }
    return 0;
  }

  void refresh_model() {
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
};

} // namespace taranis
