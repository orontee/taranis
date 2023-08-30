#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <inkview.h>
#include <memory>
#include <vector>

#include "fonts.h"
#include "icons.h"

#define T(x) GetLangText(x)

namespace taranis {

enum menu_item_index {
  MENU_ITEM_REFRESH = 100,
  MENU_ITEM_ABOUT = 200,
  MENU_ITEM_QUIT = 201,
};

class MenuButton {
public:
  static const int width{100};
  static const int height{100};
  static const int icon_size{70};
  static const int border_size{2};
  static const int border_radius{5};
  static const int border_style{ROUND_DEFAULT};

  MenuButton(int pos_x, int pos_y, std::shared_ptr<Icons> icons,
             std::shared_ptr<Fonts> fonts)
      : activated{false}, items{imenu{ITEM_ACTIVE, taranis::MENU_ITEM_REFRESH,
                                      const_cast<char *>(T("Refresh")),
                                      nullptr},
                                imenu{ITEM_ACTIVE, taranis::MENU_ITEM_ABOUT,
                                      const_cast<char *>(T("About…")), nullptr},
                                imenu{ITEM_ACTIVE, taranis::MENU_ITEM_QUIT,
                                      const_cast<char *>(T("Quit")), nullptr},
                                imenu{0, 0, nullptr, nullptr}},
        bounding_box{pos_x, pos_y, MenuButton::width, MenuButton::height},
        icon{BitmapStretchProportionally(
            icons->get("menu"), MenuButton::icon_size, MenuButton::icon_size)},
        font{fonts->get_normal_font()} {
    this->initialize_menu_position();
  }

  const irect get_bounding_box() const { return this->bounding_box; }

  void show() {
    FillAreaRect(&this->bounding_box, WHITE);

    const int icon_pos_x =
        this->bounding_box.x + (MenuButton::width - MenuButton::icon_size) / 2;
    const int icon_pos_y =
        this->bounding_box.y + (MenuButton::height - MenuButton::icon_size) / 2;
    DrawBitmap(icon_pos_x, icon_pos_y, this->icon);
  }

  bool is_activated() const { return this->activated; }

  void activate() {
    if (this->activated) {
      return;
    }
    this->activated = true;
    ;

    const int icon_center_pos_x = this->bounding_box.x + MenuButton::width / 2;
    const int icon_center_pos_y = this->bounding_box.y + MenuButton::height / 2;
    const int radius = MenuButton::width / 2;
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

  void open_menu(iv_menuhandler handler) {
    SetMenuFont(this->font.get());
    OpenMenu(const_cast<imenu *>(this->items.data()), 0, this->menu_pos_x,
             this->menu_pos_y, handler);
  }

private:
  static constexpr int font_size{40};

  bool activated;
  const std::array<imenu, 4> items;
  const irect bounding_box;
  const ibitmap *const icon;
  std::shared_ptr<ifont> font;

  int menu_pos_x;
  int menu_pos_y;

  void initialize_menu_position() {
    std::vector<int> text_widths;
    for (auto item : this->items) {
      if (item.text != nullptr) {
        text_widths.push_back(std::strlen(item.text));
      }
    }
    this->menu_pos_x =
        this->bounding_box.x + MenuButton::width -
        *std::max_element(text_widths.begin(), text_widths.end()) *
            MenuButton::font_size;
    this->menu_pos_y = this->bounding_box.y + this->bounding_box.h;
  }
};
} // namespace taranis
