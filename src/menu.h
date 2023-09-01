#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <experimental/optional>
#include <functional>
#include <inkview.h>
#include <memory>
#include <vector>

#include "fonts.h"
#include "icons.h"
#include "widget.h"

#define T(x) GetLangText(x)

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}
namespace taranis {

enum menu_item_index {
  MENU_ITEM_REFRESH = 100,
  MENU_ITEM_ABOUT = 200,
  MENU_ITEM_QUIT = 201,
};

class MenuButton : public Widget {
public:
  MenuButton(std::shared_ptr<Icons> icons, std::shared_ptr<Fonts> fonts)
      : activated{false}, items{imenu{ITEM_ACTIVE, taranis::MENU_ITEM_REFRESH,
                                      const_cast<char *>(T("Refresh")),
                                      nullptr},
                                imenu{ITEM_ACTIVE, taranis::MENU_ITEM_ABOUT,
                                      const_cast<char *>(T("About…")), nullptr},
                                imenu{ITEM_ACTIVE, taranis::MENU_ITEM_QUIT,
                                      const_cast<char *>(T("Quit")), nullptr},
                                imenu{0, 0, nullptr, nullptr}},
        icon{BitmapStretchProportionally(
            icons->get("menu"), MenuButton::icon_size, MenuButton::icon_size)},
        font{fonts->get_normal_font()} {
    const int width = MenuButton::icon_size + 2 * MenuButton::padding;
    const int height = MenuButton::icon_size + 2 * MenuButton::padding;
    this->set_bounding_box(ScreenWidth() - width, 0, width, height);
  }

  void set_menu_handler(iv_menuhandler handler) {
    this->menu_handler = handler;
  }

  void show() override {
    this->fill_bounding_box();

    const int icon_pos_x = this->bounding_box.x +
                           (this->bounding_box.w - MenuButton::icon_size) / 2;
    const int icon_pos_y = this->bounding_box.y +
                           (this->bounding_box.h - MenuButton::icon_size) / 2;
    DrawBitmap(icon_pos_x, icon_pos_y, this->icon);
  }

  bool is_activated() const { return this->activated; }

  void activate() {
    if (this->activated) {
      return;
    }
    this->activated = true;

    const int icon_center_pos_x =
        this->bounding_box.x + this->bounding_box.w / 2;
    const int icon_center_pos_y =
        this->bounding_box.y + this->bounding_box.h / 2;
    const int radius = (this->bounding_box.w - padding) / 2;
    auto canvas = GetCanvas();
    invertCircle(icon_center_pos_x, icon_center_pos_y, radius, canvas);

    PartialUpdate(this->bounding_box.x, this->bounding_box.y,
                  this->bounding_box.w, this->bounding_box.h);
  }

  void desactivate() {
    if (not this->activated) {
      return;
    }
    this->activated = false;

    this->show();

    PartialUpdate(this->bounding_box.x, this->bounding_box.y,
                  this->bounding_box.w, this->bounding_box.h);
  }

  void open_menu() {
    if (not this->menu_handler)
      return;

    const auto &[pos_x, pos_y] = this->get_menu_position();
    SetMenuFont(this->font.get());
    OpenMenu(const_cast<imenu *>(this->items.data()), 0, pos_x, pos_y,
             *this->menu_handler);
  }

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override {
    if (event_type == EVT_POINTERDOWN) {
      this->activate();
      return 1;
    }

    if (event_type == EVT_POINTERDRAG) {
      this->desactivate();
      return 0;
    }

    if (event_type == EVT_POINTERUP) {
      this->desactivate();
      this->open_menu();
      return 1;
    }
    return 0;
  }

  int handle_key_pressed(int key) override {
    if (key == IV_KEY_MENU) {
      this->open_menu();
      return 1;
    }
    return 0;
  }

private:
  static const int padding{50};
  static const int icon_size{70};
  static const int border_size{2};
  static const int border_radius{5};
  static const int border_style{ROUND_DEFAULT};

  bool activated;
  const std::array<imenu, 4> items;

  const ibitmap *const icon;
  std::shared_ptr<ifont> font;

  std::optional<iv_menuhandler> menu_handler;

  std::pair<int, int> get_menu_position() const {
    std::vector<int> text_widths;
    for (auto item : this->items) {
      if (item.text != nullptr) {
        text_widths.push_back(std::strlen(item.text));
      }
    }
    const int pos_x =
        this->bounding_box.x + this->bounding_box.w -
        *std::max_element(text_widths.begin(), text_widths.end()) *
            this->font->size;
    const int pos_y = this->bounding_box.y + this->bounding_box.h;
    return {pos_x, pos_y};
  }
};
} // namespace taranis
