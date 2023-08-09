#pragma once

#include <cstring>
#include <ctime>
#include <inkview.h>
#include <memory>
#include <string>

#include "config.h"
#include "model.h"
#include "service.h"
#include "ui.h"

using namespace std::string_literals;

#define APP_EVT_MODEL_UPDATED 1

namespace taranis {

  class App {
  private:
    std::unique_ptr<Config> config;
    std::shared_ptr<Model> model;
    std::unique_ptr<Service> service;
    std::unique_ptr<Ui> ui;

  public:
    App() {
      this->config = std::make_unique<Config>();
      this->model = std::make_shared<Model>();
      this->service = std::make_unique<Service>();
    }

    int process_loop_event(int event_type, int param_one, int param_two) {
      try {
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

        if (event_type == EVT_CUSTOM) {
          if (param_one == APP_EVT_MODEL_UPDATED) {
            if (this->ui) {
              this->ui->show();
            }
          }
        }
      } catch (const std::exception &e) {
        Message(ICON_WARNING, "Error while processing event loop", e.what(),
                1200);
      }
      return 0;
    }

  private:
    void setup() {
      this->ui = std::make_unique<Ui>(this->model);
    }

    void show() {
      if (not this->ui) return;
 
      this->load_config();
      this->ui->show();
      this->refresh_model();
    }

    void exit() {
      //this->write_config();

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

      this->model->source = "OpenWeather";
    }

    void write_config() {
      this->config->write_string("location_town"s, this->model->location.town);

      this->config->write_string("location_country"s,
                                 this->model->location.country);

      this->config->write_string("openweather_api_key"s,
                                 this->service->get_api_key());

      const auto failed = this->config->save();
      if (failed) {
        Message(ICON_WARNING, "Configuration", "Failed to write configuration",
                1200);
      }
    }

    int handle_key_pressed(int key) {
      if (key == IV_KEY_HOME) {
        this->exit();
        return 1;
      }

      if (key == IV_KEY_MENU) {
        this->refresh_model();
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

    void refresh_model() {
      ShowHourglassForce();

      this->model->refresh_date = std::time(nullptr);

      auto conditions = this->service->fetch_data(this->model->location.town,
                                                  this->model->location.country);
      this->model->current_condition = conditions.front();
      auto it = std::begin(conditions);
      ++it; // skip current
      while (it != std::end(conditions)) {
        this->model->hourly_forecast.push_back(*it);
        ++it;
      }

      HideHourglass();

      const auto event_handler = GetEventHandler();
      SendEvent(event_handler, EVT_CUSTOM, APP_EVT_MODEL_UPDATED, 0);
    }
  };

}
