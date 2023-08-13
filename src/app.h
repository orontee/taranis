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
#include "l10n.h"
#include "model.h"
#include "service.h"
#include "ui.h"

#define T(x) GetLangText(x)

#define APP_EVT_MODEL_UPDATED 1
#define APP_EVT_REFRESH_REQUESTED 2
#define APP_EVT_ABOUT_DIALOG_REQUESTED 3

using namespace std::placeholders;
using namespace std::string_literals;

namespace taranis {

void handle_menu_item_selected(int index);

class App {
public:
  App()
      : config{std::make_unique<Config>()}, model{std::make_shared<Model>()},
        service{std::make_unique<Service>()} {}

  int process_event(int event_type, int param_one, int param_two) {
    if (event_type == EVT_INIT) {
      this->setup();
      return 1;
    }

    if (event_type == EVT_SHOW) {
      this->show();
      return 1;
    }

    if (event_type == EVT_KEYPRESS) {
      return this->handle_key_pressed(param_one);
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

    if (event_type == EVT_POINTERDOWN) {
      return this->handle_pointer_down_event(param_one, param_two);
    }

    if (event_type == EVT_POINTERDRAG) {
      return this->handle_pointer_drag_event(param_one, param_two);
    }

    if (event_type == EVT_POINTERUP) {
      return this->handle_pointer_up_event(param_one, param_two);
    }
    return 0;
  }

private:
  static const int error_dialog_delay{3600};

  std::unique_ptr<Config> config;
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
    this->config.reset();

    CloseApp();
  }

  void load_config() {
    this->model->location.town =
        this->config->read_string("location_town"s, "Paris"s);

    this->model->location.country =
        this->config->read_string("location_country"s, "France"s);

    this->service->set_api_key(this->config->read_string(
        "openweather_api_key"s, "4620ad6f20069b66bc36b1e88ceb4541"s));
    // API key associated to open-source plan

    this->model->source = "OpenWeather";

    initialize_translations();
  }

  void write_config() {
    this->config->write_string("location_town"s, this->model->location.town);

    this->config->write_string("location_country"s,
                               this->model->location.country);

    this->config->write_string("openweather_api_key"s,
                               this->service->get_api_key());

    this->config->save();
  }

  int handle_key_pressed(int key) {
    if (key == IV_KEY_HOME) {
      this->exit();
      return 1;
    }

    if (key == IV_KEY_MENU) {
      this->open_menu();
      return 1;
    }

    if (key == IV_KEY_PREV) {
      if (this->ui) {
        this->ui->decrease_forecast_offset();
        return 1;
      }
    }

    if (key == IV_KEY_NEXT) {
      if (this->ui) {
        this->ui->increase_forecast_offset();
        return 1;
      }
    }

    return 0;
  }

  int handle_custom_event(int param_one, int param_two) {
    if (param_one == APP_EVT_MODEL_UPDATED) {
      if (this->ui) {
        this->ui->show();
        return 1;
      }
    } else if (param_one == APP_EVT_REFRESH_REQUESTED) {
      this->refresh_model();
      return 1;
    }
    return 0;
  }

  int handle_pointer_down_event(int pointer_pos_x, int pointer_pos_y) {
    if (this->ui) {
      const auto menu_button_bounding_box =
          this->ui->get_menu_button()->get_bounding_box();

      if (IsInRect(pointer_pos_x, pointer_pos_y, &menu_button_bounding_box)) {
        this->ui->get_menu_button()->activate();
        return 1;
      }
    }
    return 0;
  }

  int handle_pointer_drag_event(int pointer_pos_x, int pointer_pos_y) {
    if (this->ui) {
      this->ui->get_menu_button()->desactivate();
    }
    return 0;
  }

  int handle_pointer_up_event(int pointer_pos_x, int pointer_pos_y) {
    if (this->ui) {
      const auto menu_button_bounding_box =
          this->ui->get_menu_button()->get_bounding_box();

      if (IsInRect(pointer_pos_x, pointer_pos_y, &menu_button_bounding_box)) {
        this->ui->get_menu_button()->desactivate();
        this->open_menu();
        return 1;
      }
    }
    return 0;
  }

  void refresh_model() {
    ShowHourglassForce();

    this->model->refresh_date = std::time(nullptr);

    try {
      std::vector<Condition> conditions = this->service->fetch_conditions(
          this->model->location.town, this->model->location.country);

      this->model->current_condition = conditions.front();

      auto it = std::begin(conditions);
      ++it; // skip current
      while (it != std::end(conditions)) {
        this->model->hourly_forecast.push_back(*it);
        ++it;
      }

      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM, APP_EVT_MODEL_UPDATED, 0);
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

  void open_menu() {
    auto menu_button = this->ui ? this->ui->get_menu_button() : nullptr;
    if (not menu_button) {
      return;
    }

    menu_button->open_menu(handle_menu_item_selected);
  }
};

} // namespace taranis
