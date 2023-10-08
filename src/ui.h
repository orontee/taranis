#pragma once

#include <inkview.h>
#include <memory>
#include <vector>

#include "alerts.h"
#include "fonts.h"
#include "hourlyforecastbox.h"
#include "icons.h"
#include "model.h"
#include "swipe.h"

namespace taranis {

class Ui {
public:
  // ⚠️ Must be instantiated after application received the EVT_INIT
  // event otherwise opening fonts, etc. fails

  Ui(std::shared_ptr<Model> model);

  void show();

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y);

  int handle_key_pressed(int key);

  void display_alert() { this->alerts_button->open_dialog_maybe(); }

  void switch_forecast_widget();

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  SwipeDetector swipe_detector;

  std::shared_ptr<AlertsButton> alerts_button;

  std::shared_ptr<HourlyForecastBox> hourly_forecast_box;
  std::shared_ptr<Widget> daily_forecast_box;

  std::vector<std::shared_ptr<Widget>> children;

  const int alert_icon_size = 150;
  const int button_margin = 25;

  static bool is_on_widget(int pointer_pos_x, int pointer_pos_y,
                           std::shared_ptr<Widget> widget);

  std::shared_ptr<Widget> get_forecast_widget() const;

  void select_forecast_widget();

  int handle_possible_swipe(int event_type, int pointer_pos_x,
                            int pointer_pos_y);

  static void handle_menu_item_selected(int item_index);
};

} // namespace taranis
