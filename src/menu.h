#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <inkview.h>
#include <map>
#include <memory>
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
  MENU_ITEM_EDIT_LOCATION = 201,
  MENU_ITEM_TOGGLE_FAVORITE_LOCATION = 202,
  MENU_ITEM_CONFIGURE = 203,
  MENU_ITEM_QUIT = 300,
  MENU_ITEM_EMPTY_LOCATION_HISTORY = 800,
};

class MenuButton : public Button {
public:
  MenuButton(int icon_size, std::shared_ptr<Model> model,
             std::shared_ptr<Icons> icons, std::shared_ptr<Fonts> fonts);

  void set_menu_handler(iv_menuhandler handler);

  bool handle_key_press(int key) override;

  bool handle_key_release(int key) override;

protected:
  void on_clicked() override { this->open_menu(); }

private:
  typedef std::array<imenuex, 5> MenuItems;

  static const int padding{5};

  std::shared_ptr<Icons> icons;

  std::array<imenuex, LocationHistoryProxy::max_size + 3>
      location_history_items;
  MenuItems items;

  std::vector<std::string> location_full_names;

  std::shared_ptr<Model> model;
  std::shared_ptr<ifont> font;
  std::unique_ptr<LocationHistoryProxy> history;

  std::unique_ptr<ibitmap> favorite_location_icon;

  iv_menuhandler menu_handler;

  std::pair<int, int> get_menu_position() const;

  void initialize_favorite_location_icon();

  void update_item_texts();

  void update_location_history_items();

  void open_menu();
};
} // namespace taranis
