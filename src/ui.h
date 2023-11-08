#pragma once

#include <experimental/optional>
#include <inkview.h>
#include <memory>
#include <vector>

#include "alerts.h"
#include "fonts.h"
#include "hourlyforecastbox.h"
#include "icons.h"
#include "keys.h"
#include "locationselector.h"
#include "model.h"
#include "swipe.h"

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {

class Ui : public KeyEventDispatcher {
public:
  // ⚠️ Must be instantiated after application received the EVT_INIT
  // event otherwise opening fonts, etc. fails

  Ui(std::shared_ptr<Model> model);

  virtual ~Ui() {}

  void paint();

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y);

  void display_alert() { this->alert_viewer->open(); }

  void switch_forecast_widget();

  void open_location_list(const std::vector<Location> &locations);

  std::optional<Location> get_location_from_location_list(size_t index) const;

protected:
  bool is_consumer_active(std::shared_ptr<KeyEventConsumer> consumer) override;

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  SwipeDetector swipe_detector;

  std::shared_ptr<AlertViewer> alert_viewer;

  std::shared_ptr<HourlyForecastBox> hourly_forecast_box;
  std::shared_ptr<Widget> daily_forecast_box;

  std::shared_ptr<LocationSelector> location_selector;

  std::vector<std::shared_ptr<Widget>> children;
  std::vector<std::shared_ptr<Widget>> modals;

  std::shared_ptr<Widget> visible_modal{nullptr};

  const int alert_icon_size = 150;
  const int button_margin = 25;

  static bool is_on_widget(int pointer_pos_x, int pointer_pos_y,
                           std::shared_ptr<Widget> widget);

  void check_modal_visibility();

  std::shared_ptr<Widget> get_forecast_widget() const;

  void select_forecast_widget();

  int handle_possible_swipe(int event_type, int pointer_pos_x,
                            int pointer_pos_y);

  static void handle_menu_item_selected(int item_index);
};

} // namespace taranis
