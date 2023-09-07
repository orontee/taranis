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
#include "icons.h"
#include "model.h"

#define T(x) GetLangText(x)

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}
namespace taranis {

enum menu_item_index {
  MENU_ITEM_REFRESH = 100,
  MENU_ITEM_UNIT_SYSTEM = 200,
  MENU_ITEM_ABOUT = 201,
  MENU_ITEM_QUIT = 300,
  MENU_ITEM_UNIT_SYSTEM_STANDARD = 600,
  MENU_ITEM_UNIT_SYSTEM_METRIC = 601,
  MENU_ITEM_UNIT_SYSTEM_IMPERIAL = 602,
};

class MenuButton : public Button {
public:
  MenuButton(std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
             std::shared_ptr<Fonts> fonts)
      : Button {
	MenuButton::icon_size + 2 * MenuButton::padding,
	MenuButton::icon_size + 2 * MenuButton::padding,
	BitmapStretchProportionally(icons->get("menu"),
                                    MenuButton::icon_size,
                                    MenuButton::icon_size)
      }, unit_system_items{imenu{ITEM_ACTIVE, MENU_ITEM_UNIT_SYSTEM_STANDARD,
                                const_cast<char *>(T("Standard")), nullptr},
                          imenu{ITEM_ACTIVE, MENU_ITEM_UNIT_SYSTEM_METRIC,
                                const_cast<char *>(T("Metric")), nullptr},
                          imenu{ITEM_ACTIVE, MENU_ITEM_UNIT_SYSTEM_IMPERIAL,
                                const_cast<char *>(T("Imperial")), nullptr},
                          imenu{0, 0, nullptr, nullptr}},
        items{imenu{ITEM_ACTIVE, taranis::MENU_ITEM_REFRESH,
                    const_cast<char *>(T("Refresh")), nullptr},
              imenu{ITEM_SUBMENU, taranis::MENU_ITEM_UNIT_SYSTEM,
                    const_cast<char *>(T("Units")),
                    const_cast<imenu *>(unit_system_items.data())},
              imenu{ITEM_ACTIVE, taranis::MENU_ITEM_ABOUT,
                    const_cast<char *>(T("About…")), nullptr},
              imenu{ITEM_ACTIVE, taranis::MENU_ITEM_QUIT,
                    const_cast<char *>(T("Quit")), nullptr},
              imenu{0, 0, nullptr, nullptr}},
        model{model},
        font{fonts->get_normal_font()} {
    this->set_pos_x(ScreenWidth() - this->get_width());
    this->set_pos_y(0);
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
  void on_clicked() override {
    this->open_menu();
  }

private:
  static const int padding{50};
  static const int icon_size{70};

  std::array<imenu, 4> unit_system_items;
  const std::array<imenu, 5> items;

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
    const int pos_x = this->bounding_box.x + this->bounding_box.w -
                      *std::max_element(text_widths.begin(), text_widths.end());
    const int pos_y =
        this->bounding_box.y + this->bounding_box.h - MenuButton::padding;
    return {pos_x, pos_y};
  }

  void update_unit_system_bullet() const {
    if (this->model->unit_system == UnitSystem::standard) {
      const_cast<imenu *>(&this->unit_system_items[0])->type = ITEM_BULLET;
      const_cast<imenu *>(&this->unit_system_items[1])->type = ITEM_ACTIVE;
      const_cast<imenu *>(&this->unit_system_items[2])->type = ITEM_ACTIVE;
    } else if (this->model->unit_system == UnitSystem::metric) {
      const_cast<imenu *>(&this->unit_system_items[0])->type = ITEM_ACTIVE;
      const_cast<imenu *>(&this->unit_system_items[1])->type = ITEM_BULLET;
      const_cast<imenu *>(&this->unit_system_items[2])->type = ITEM_ACTIVE;
    } else if (this->model->unit_system == UnitSystem::imperial) {
      const_cast<imenu *>(&this->unit_system_items[0])->type = ITEM_ACTIVE;
      const_cast<imenu *>(&this->unit_system_items[1])->type = ITEM_ACTIVE;
      const_cast<imenu *>(&this->unit_system_items[2])->type = ITEM_BULLET;
    }
  }

  void open_menu() {
    if (not this->menu_handler)
      return;

    this->update_unit_system_bullet();

    const auto &[pos_x, pos_y] = this->get_menu_position();
    SetMenuFont(this->font.get());
    OpenMenu(const_cast<imenu *>(this->items.data()), 0, pos_x, pos_y,
             *this->menu_handler);
  }

};
} // namespace taranis
