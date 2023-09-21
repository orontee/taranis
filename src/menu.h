#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <experimental/optional>
#include <functional>
#include <inkview.h>
#include <memory>
#include <vector>

#include "button.h"
#include "fonts.h"
#include "history.h"
#include "icons.h"
#include "model.h"
#include "util.h"

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}
namespace taranis {

enum menu_item_index {
  MENU_ITEM_REFRESH = 100,
  MENU_ITEM_LOCATION_HISTORY = 200,
  MENU_ITEM_UNIT_SYSTEM = 202,
  MENU_ITEM_ABOUT = 203,
  MENU_ITEM_QUIT = 300,
  MENU_ITEM_UNIT_SYSTEM_STANDARD = 600,
  MENU_ITEM_UNIT_SYSTEM_METRIC = 601,
  MENU_ITEM_UNIT_SYSTEM_IMPERIAL = 602,
  MENU_ITEM_EMPTY_LOCATION_HISTORY = 800,
};

class MenuButton : public Button {
  typedef std::array<imenu, 6> MenuItems;

public:
  MenuButton(std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
             std::shared_ptr<Fonts> fonts)
      : Button{MenuButton::icon_size, icons->get("menu")},
        unit_system_items{
            imenu{ITEM_ACTIVE, MENU_ITEM_UNIT_SYSTEM_STANDARD,
                  const_cast<char *>(GetLangText("Standard")), nullptr},
            imenu{ITEM_ACTIVE, MENU_ITEM_UNIT_SYSTEM_METRIC,
                  const_cast<char *>(GetLangText("Metric")), nullptr},
            imenu{ITEM_ACTIVE, MENU_ITEM_UNIT_SYSTEM_IMPERIAL,
                  const_cast<char *>(GetLangText("Imperial")), nullptr},
            imenu{0, 0, nullptr, nullptr}},
        items{imenu{ITEM_ACTIVE, taranis::MENU_ITEM_REFRESH,
                    const_cast<char *>(GetLangText("Refresh")), nullptr},
              imenu{ITEM_SUBMENU, taranis::MENU_ITEM_LOCATION_HISTORY,
                    const_cast<char *>(GetLangText("Locations")),
                    this->location_history_items.data()},
              imenu{ITEM_SUBMENU, taranis::MENU_ITEM_UNIT_SYSTEM,
                    const_cast<char *>(GetLangText("Units")),
                    const_cast<imenu *>(this->unit_system_items.data())},
              imenu{ITEM_ACTIVE, taranis::MENU_ITEM_ABOUT,
                    const_cast<char *>(GetLangText("About…")), nullptr},
              imenu{ITEM_ACTIVE, taranis::MENU_ITEM_QUIT,
                    const_cast<char *>(GetLangText("Quit")), nullptr},
              imenu{0, 0, nullptr, nullptr}},
        model{model}, font{fonts->get_normal_font()} {
    this->update_location_history_items();
  }

  void set_menu_handler(iv_menuhandler handler) {
    this->menu_handler = handler;
  }

  int handle_key_pressed(int key) override {
    if (key == IV_KEY_MENU) {
      this->open_menu();
      return 1;
    }
    return 0;
  }

protected:
  void on_clicked() override { this->open_menu(); }

private:
  static const int padding{5};
  static const int icon_size{70};

  std::array<imenu, 4> unit_system_items;
  std::array<imenu, LocationHistory::max_size + 1> location_history_items;
  const MenuItems items;

  std::vector<std::string> location_full_names;

  std::shared_ptr<Model> model;

  std::shared_ptr<ifont> font;

  std::optional<iv_menuhandler> menu_handler;

  std::pair<int, int> get_menu_position() const {
    SetFont(this->font.get(), BLACK);

    std::vector<int> text_widths;
    for (auto item : this->items) {
      if (item.text != nullptr) {
        text_widths.push_back(StringWidth(item.text));
      }
    }
    const int pos_x = this->bounding_box.x -
                      *std::max_element(text_widths.begin(), text_widths.end());
    const int pos_y =
        this->bounding_box.y + this->bounding_box.h - MenuButton::padding;
    return {pos_x, pos_y};
  }

  void update_location_history_items() {
    this->location_full_names.clear();

    size_t index = 0;
    auto &location_history = this->model->location_history;
    if (location_history.empty()) {
      this->location_history_items.at(index) =
          imenu{ITEM_INACTIVE, taranis::MENU_ITEM_EMPTY_LOCATION_HISTORY,
                const_cast<char *>(GetLangText("Empty")), nullptr};
      ++index;
    } else {
      for (const auto &location : location_history) {
        this->location_full_names.push_back(format_location(location));

        const short item_index =
            taranis::MENU_ITEM_EMPTY_LOCATION_HISTORY + index + 1;
        this->location_history_items.at(index) =
            imenu{ITEM_ACTIVE, item_index, nullptr, nullptr};

        ++index;
        if (index >= this->location_history_items.size() - 1) {
          break;
        }
      }
    }
    while (index < this->location_history_items.size()) {
      this->location_history_items.at(index) = imenu{0, 0, nullptr, nullptr};
      ++index;
    }
    index = 0;
    for (const auto &item_name : this->location_full_names) {
      this->location_history_items.at(index).text =
          const_cast<char *>(item_name.c_str());
      ++index;
    }
  }

  void update_unit_system_bullet() {
    if (this->model->unit_system == UnitSystem::standard) {
      this->unit_system_items[0].type = ITEM_BULLET;
      this->unit_system_items[1].type = ITEM_ACTIVE;
      this->unit_system_items[2].type = ITEM_ACTIVE;
    } else if (this->model->unit_system == UnitSystem::metric) {
      this->unit_system_items[0].type = ITEM_ACTIVE;
      this->unit_system_items[1].type = ITEM_BULLET;
      this->unit_system_items[2].type = ITEM_ACTIVE;
    } else if (this->model->unit_system == UnitSystem::imperial) {
      this->unit_system_items[0].type = ITEM_ACTIVE;
      this->unit_system_items[1].type = ITEM_ACTIVE;
      this->unit_system_items[2].type = ITEM_BULLET;
    }
  }

  void open_menu() {
    if (not this->menu_handler)
      return;

    this->update_location_history_items();
    this->update_unit_system_bullet();

    const auto &[pos_x, pos_y] = this->get_menu_position();
    SetMenuFont(this->font.get());
    OpenMenu(const_cast<imenu *>(this->items.data()), 0, pos_x, pos_y,
             *this->menu_handler);
  }
};
} // namespace taranis
