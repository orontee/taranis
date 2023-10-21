#include "menu.h"

#include "experimental/optional"
#include "inkview.h"
#include "util.h"

namespace taranis {

MenuButton::MenuButton(std::shared_ptr<Model> model,
                       std::shared_ptr<Icons> icons,
                       std::shared_ptr<Fonts> fonts)
    : Button{MenuButton::icon_size, icons->get("menu")}, icons{icons},
      items{imenuex{ITEM_ACTIVE, taranis::MENU_ITEM_REFRESH, nullptr, nullptr,
                    nullptr, nullptr, nullptr},
            imenuex{ITEM_SUBMENU, taranis::MENU_ITEM_LOCATION_HISTORY, nullptr,
                    this->location_history_items.data(), nullptr, nullptr,
                    nullptr},
            imenuex{ITEM_ACTIVE, taranis::MENU_ITEM_CONFIGURE, nullptr, nullptr,
                    nullptr, nullptr, nullptr},
            imenuex{ITEM_ACTIVE, taranis::MENU_ITEM_ABOUT, nullptr, nullptr,
                    nullptr, nullptr, nullptr},
            imenuex{ITEM_ACTIVE, taranis::MENU_ITEM_QUIT, nullptr, nullptr,
                    nullptr, nullptr, nullptr},
            imenuex{0, 0, nullptr, nullptr, nullptr, nullptr, nullptr}},
      model{model}, font{fonts->get_normal_font()},
      history{std::make_unique<LocationHistoryProxy>(this->model)},
      menu_handler{nullptr} {
  this->initialize_favorite_location_icon();
  this->update_location_history_items();
}

void MenuButton::set_menu_handler(iv_menuhandler handler) {
  this->menu_handler = handler;
}

bool MenuButton::handle_key_press(int key) { return key == IV_KEY_MENU; }

bool MenuButton::handle_key_release(int key) {
  if (key == IV_KEY_MENU) {
    this->open_menu();
    return true;
  }
  return false;
}

std::pair<int, int> MenuButton::get_menu_position() const {
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

void MenuButton::initialize_favorite_location_icon() {
  const auto menu_properties = LoadContextMenuProperties();
  const auto menu_font_height = menu_properties and menu_properties->font_normal
                                    ? menu_properties->font_normal->height
                                    : this->font->height * 0.9;
  this->favorite_location_icon = BitmapStretchProportionally(
      this->icons->get("favorite"), menu_font_height, menu_font_height);
}

void MenuButton::update_item_texts() {
  for (auto &item : this->items) {
    if (item.index == MENU_ITEM_REFRESH) {
      item.text = const_cast<char *>(GetLangText("Refresh"));
    } else if (item.index == MENU_ITEM_LOCATION_HISTORY) {
      item.text = const_cast<char *>(GetLangText("Locations"));
    } else if (item.index == MENU_ITEM_CONFIGURE) {
      item.text = const_cast<char *>(GetLangText("Parameters…"));
    } else if (item.index == MENU_ITEM_ABOUT) {
      item.text = const_cast<char *>(GetLangText("About…"));
    } else if (item.index == MENU_ITEM_QUIT) {
      item.text = const_cast<char *>(GetLangText("Quit"));
    }
  }
}

void MenuButton::update_location_history_items() {
  this->location_full_names.clear();

  size_t index = 0;
  auto &location_history = this->model->location_history;
  if (location_history.empty()) {
    this->location_history_items.at(index) =
        imenuex{ITEM_INACTIVE,
                taranis::MENU_ITEM_EMPTY_LOCATION_HISTORY,
                const_cast<char *>(GetLangText("Empty")),
                nullptr,
                nullptr,
                nullptr,
                nullptr};
    ++index;

    this->location_history_items[index] =
        imenuex{ITEM_INACTIVE, taranis::MENU_ITEM_TOGGLE_FAVORITE_LOCATION,
                nullptr,       nullptr,
                nullptr,       nullptr,
                nullptr};
    this->location_full_names.push_back(
        const_cast<char *>(GetLangText("Add to favorites")));
    ++index;
  } else {
    for (const auto &item : location_history) {
      this->location_full_names.push_back(format_location(item.location));

      ibitmap *icon = item.favorite
                          ? const_cast<ibitmap *>(this->favorite_location_icon)
                          : nullptr;
      const short item_index =
          taranis::MENU_ITEM_EMPTY_LOCATION_HISTORY + index + 1;
      this->location_history_items.at(index) = imenuex{
          ITEM_ACTIVE, item_index, nullptr, nullptr, icon, nullptr, nullptr};

      ++index;
      if (index == location_history.size() or
          index == this->location_history_items.size() - 2) {
        const auto toggle_item_text =
            const_cast<char *>(history->is_current_location_favorite()
                                   ? GetLangText("Remove from favorites")
                                   : GetLangText("Add to favorites"));

        const bool is_toggle_item_active =
            history->is_current_location_favorite() or
            history->can_add_favorite();

        const short toggle_item_state =
            is_toggle_item_active ? ITEM_ACTIVE : ITEM_INACTIVE;

        this->location_history_items.at(index) =
            imenuex{toggle_item_state,
                    taranis::MENU_ITEM_TOGGLE_FAVORITE_LOCATION,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr};
        this->location_full_names.push_back(toggle_item_text);

        ++index;
        break;
      }
    }
  }
  while (index < this->location_history_items.size()) {
    this->location_history_items.at(index) =
        imenuex{0, 0, nullptr, nullptr, nullptr, nullptr, nullptr};
    ++index;
  }
  index = 0;
  for (const auto &item_name : this->location_full_names) {
    this->location_history_items.at(index).text =
        const_cast<char *>(item_name.c_str());
    ++index;
  }
}

void MenuButton::open_menu() {
  if (not this->menu_handler) {
    return;
  }
  if (this->model->refresh_date == std::experimental::nullopt) {
    // don't popup menu while refreshing due to display update bug
    // after menu dialog is hidden... See issue #51
    return;
  }

  this->update_item_texts();
  this->update_location_history_items();

  const auto &[pos_x, pos_y] = this->get_menu_position();
  SetMenuFont(this->font.get());
  OpenMenuEx(const_cast<imenuex *>(this->items.data()), 0, pos_x, pos_y,
             *this->menu_handler);
}

} // namespace taranis
