#pragma once

#include <experimental/optional>
#include <inkview.h>
#include <memory>
#include <vector>

#include "alerts.h"
#include "config.h"
#include "fonts.h"
#include "forecast.h"
#include "icons.h"
#include "keys.h"
#include "locationbox.h"
#include "locationselector.h"
#include "model.h"

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {

class Ui : public KeyEventDispatcher {
public:
  // ⚠️ Must be instantiated after application received the EVT_INIT
  // event otherwise opening fonts, etc. fails

  Ui(std::shared_ptr<Config> config, std::shared_ptr<Model> model);

  virtual ~Ui() {}

  void paint();

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y);

  bool is_key_event_consumer_active(
      std::shared_ptr<KeyEventConsumer> consumer) override;

  void display_alert() { this->alert_viewer->open(); }

  void switch_forecast_widget();

  void edit_location() { this->location_box->edit_location(); }

  void open_location_list(const std::vector<Location> &locations);

  std::optional<Location> get_location_from_location_list(size_t index) const;

  void generate_logo_maybe() const;

private:
  static constexpr int alert_button_icon_size{70};
  static constexpr int menu_button_icon_size{70};
  static constexpr int location_selector_icon_size{50};
  static constexpr int button_margin{25};

  std::shared_ptr<Config> config;
  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  std::shared_ptr<LocationBox> location_box;
  std::shared_ptr<AlertViewer> alert_viewer;

  std::shared_ptr<ForecastStack> forecast_stack;

  std::shared_ptr<LocationSelector> location_selector;

  std::vector<std::shared_ptr<Widget>> children;
  std::vector<std::shared_ptr<ModalWidget>> modals;

  std::shared_ptr<ModalWidget> visible_modal{nullptr};

  static bool is_on_widget(int pointer_pos_x, int pointer_pos_y,
                           std::shared_ptr<Widget> widget);

  void check_modal_visibility();

  static void handle_menu_item_selected(int item_index);
};

} // namespace taranis
