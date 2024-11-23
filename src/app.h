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

#include "config.h"
#include "events.h"
#include "history.h"
#include "http.h"
#include "l10n.h"
#include "model.h"
#include "service.h"
#include "state.h"
#include "ui.h"
#include "version.h"

using namespace std::placeholders;
using namespace std::string_literals;

namespace taranis {

std::string get_about_content();
// defined in generated file about.cc

class App {
public:
  App();

  int process_event(int event_type, int param_one, int param_two);

private:
  static constexpr char refresh_timer_name[] = "taranis_refresh_timer";
  static constexpr int refresh_period{60 * 60 * 1000};
  static constexpr int error_dialog_delay{5000};
  static constexpr int task_icon_size{80};

  std::shared_ptr<HttpClient> client;
  std::shared_ptr<Model> model;
  std::shared_ptr<Config> config;
  std::unique_ptr<L10n> l10n;
  std::unique_ptr<Service> service;
  std::unique_ptr<VersionChecker> version_checker;
  std::unique_ptr<LocationHistoryProxy> history;
  std::unique_ptr<ApplicationState> application_state;
  std::unique_ptr<ibitmap> task_icon;
  std::unique_ptr<Ui> ui;

  bool config_already_loaded{false};
  bool show_on_next_activate{false};

  void setup();

  void show();

  void exit();

  void load_config();

  int handle_custom_event(int param_one, int param_two);

  void clear_model_weather_conditions();

  bool can_keep_data_at_startup() const;

  bool must_skip_data_refresh(CallContext context) const;

  bool is_data_refresh_periodic() const;

  void refresh_data(CallContext context);

  void open_about_dialog();

  void search_location(const std::string &location_description);

  void set_model_location(const Location &location) const;

  void check_minimum_supported_firmware();

  void set_app_capabilities();

  void set_task_parameters();

  void start_refresh_timer() const;

  void cancel_refresh_timer() const;

  static void refresh_timer_callback();
};
} // namespace taranis
