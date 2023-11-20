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

#include "events.h"
#include "history.h"
#include "http.h"
#include "l10n.h"
#include "model.h"
#include "service.h"
#include "state.h"
#include "ui.h"

using namespace std::placeholders;
using namespace std::string_literals;

namespace taranis {

class App {
public:
  App()
      : client{std::make_shared<HttpClient>()},
        model{std::make_shared<Model>()}, l10n{std::make_unique<L10n>()},
        service{std::make_unique<Service>(client)},
        history{std::make_unique<LocationHistoryProxy>(this->model)} {}

  int process_event(int event_type, int param_one, int param_two);

private:
  static constexpr char refresh_timer_name[] = "taranis_refresh_timer";
  static constexpr int refresh_period{60 * 60 * 1000};
  static constexpr int error_dialog_delay{5000};

  std::shared_ptr<HttpClient> client;
  std::shared_ptr<Model> model;
  std::unique_ptr<L10n> l10n;
  std::unique_ptr<Service> service;
  std::unique_ptr<LocationHistoryProxy> history;
  std::unique_ptr<ApplicationState> application_state;
  std::unique_ptr<Ui> ui;

  bool config_already_loaded{false};
  bool show_on_next_activate{false};

  void setup();

  void show();

  void exit();

  void load_config();

  int handle_custom_event(int param_one, int param_two);

  void clear_model_weather_conditions();

  bool must_skip_data_refresh() const;

  void refresh_data(CallContext context);

  void search_location(const std::string &location_description);

  void set_model_location(const Location &location) const;

  void update_configured_unit_system(UnitSystem unit_system);

  static void set_task_parameters();

  static void refresh_data_maybe();
};
} // namespace taranis
