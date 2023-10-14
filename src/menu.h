#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <inkview.h>
#include <memory>
#include <map>
#include <vector>

#include "button.h"
#include "fonts.h"
#include "history.h"
#include "icons.h"
#include "model.h"

namespace taranis {

enum menu_item_index {
  MENU_ITEM_REFRESH = 100,
  MENU_ITEM_LOCATION_HISTORY = 200,
  MENU_ITEM_TOGGLE_FAVORITE_LOCATION = 201,
  MENU_ITEM_UNIT_SYSTEM = 202,
  MENU_ITEM_ABOUT = 203,
  MENU_ITEM_QUIT = 300,
  MENU_ITEM_UNIT_SYSTEM_STANDARD = 600,
  MENU_ITEM_UNIT_SYSTEM_METRIC = 601,
  MENU_ITEM_UNIT_SYSTEM_IMPERIAL = 602,
  MENU_ITEM_EMPTY_LOCATION_HISTORY = 800,
};

class MenuButton : public Button {
public:
  MenuButton(std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
             std::shared_ptr<Fonts> fonts);

  void set_menu_handler(iv_menuhandler handler);

  int handle_key_pressed(int key) override;

protected:
  void on_clicked() override { this->open_menu(); }

private:
  typedef std::array<imenuex, 6> MenuItems;

  static const int padding{5};
  static const int icon_size{70};

  std::shared_ptr<Icons> icons;

  std::array<imenuex, 4> unit_system_items;
  std::array<imenuex, LocationHistoryProxy::max_size + 2>
      location_history_items;
  MenuItems items;

  std::vector<std::string> location_full_names;

  std::shared_ptr<Model> model;
  std::shared_ptr<ifont> font;
  std::unique_ptr<LocationHistoryProxy> history;

  const ibitmap *favorite_location_icon;

  iv_menuhandler menu_handler;

  std::pair<int, int> get_menu_position() const;

  void initialize_favorite_location_icon();

  void update_item_texts();

  void update_location_history_items();

  void update_unit_system_bullet();

  void open_menu();
};
} // namespace taranis
