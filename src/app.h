#pragma once

#include <array>
#include <boost/log/trivial.hpp>
#include <cctype>
#include <cstring>
#include <ctime>
#include <inkview.h>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "history.h"
#include "model.h"
#include "service.h"
#include "state.h"
#include "ui.h"

using namespace std::placeholders;
using namespace std::string_literals;

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {

std::string get_about_content();
// defined in generated file about.cc

class App {
public:
  App()
      : model{std::make_shared<Model>()}, service{std::make_unique<Service>()},
        application_state{std::make_unique<ApplicationState>(this->model)},
        history{std::make_unique<LocationHistoryProxy>(this->model)} {}

  int process_event(int event_type, int param_one, int param_two);

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

  void setup();

  void show();

  void exit();

  void initialize_language();

  void load_config();

  int handle_custom_event(int param_one, int param_two);

  void clear_model_weather_conditions();

  void refresh_data();

  void open_about_dialog();

  void search_location(const std::string &location_description);

  void set_model_location(const Location &location) const;

  void update_configured_unit_system(UnitSystem unit_system);

  void update_configured_display_daily_forecast(bool enable) const;

  static void refresh_data_maybe();

  static void handle_about_dialog_button_clicked(int button_index);
};
} // namespace taranis
