#include "locationlist.h"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <inkview.h>

#include "events.h"
#include "util.h"

namespace {
taranis::LocationList *that;

iv_handler application_event_handler;

int last_selected_item_index;
} // namespace

namespace taranis {

LocationList::LocationList(const int icon_size, std::shared_ptr<Fonts> fonts,
                           std::shared_ptr<Icons> icons)
    : font{fonts->get_normal_font()},
      radio_button_unchecked{BitmapStretchProportionally(
          icons->get("radio-button-unchecked"), icon_size, icon_size)},
      radio_button_checked{BitmapStretchProportionally(
          icons->get("radio-button-checked"), icon_size, icon_size)} {}

std::optional<Location> LocationList::get_location(size_t index) const {
  if (index < this->locations.size()) {
    return this->locations.at(index);
  }
  return std::experimental::nullopt;
}

void LocationList::set_locations(const std::vector<Location> &locations) {
  this->locations = locations;

  this->item_names.clear();
  this->item_names.reserve(this->locations.size());

  for (const auto &location : this->locations) {
    this->item_names.push_back(format_location(location, true));
  }
}

void LocationList::show() {
  if (this->locations.size() == 0) {
    return;
  }
  that = this;
  application_event_handler = GetEventHandler();

  OpenList(GetLangText("Locations"), nullptr, this->get_item_width(),
           this->get_item_height(), this->locations.size(), 0,
           &handle_list_action);
}

int LocationList::get_item_width() const {
  return ScreenWidth() - 2 * LocationList::horizontal_padding;
}

int LocationList::get_item_height() const {
  return std::max(static_cast<int>(this->radio_button_unchecked->height),
                  this->font->height) +
         LocationList::vertical_padding;
}

int LocationList::get_icon_vertical_offset() const {
  return (get_item_height() - this->radio_button_checked->height) / 2;
}

int LocationList::handle_list_action(int action, int x, int y, int item_index,
                                     int state) {
  if (that == nullptr) {
    BOOST_LOG_TRIVIAL(debug)
        << "Skipping action received by uninitialized list " << action;
    return 1;
  }
  if (action == LIST_BEGINPAINT) {
    BOOST_LOG_TRIVIAL(debug) << "Starting to paint location list";
    return 1;
  } else if (action == LIST_PAINT) {
    if (0 <= item_index and item_index < that->item_names.size()) {
      BOOST_LOG_TRIVIAL(debug)
          << "Drawing list item with index " << item_index << " and state "
          << state << " at " << x << ", " << y;

      const ibitmap *icon = that->radio_button_unchecked;
      if (state) {
        if (last_selected_item_index != -1) {
          that->item_names[last_selected_item_index] =
              format_location(that->locations[last_selected_item_index], true);
        }
        last_selected_item_index = item_index;
        icon = that->radio_button_checked;

        DrawSelection(
            x, y, ScreenWidth() - 2 * LocationList::horizontal_padding,
            that->font->height + +LocationList::vertical_padding, BLACK);
      }
      DrawBitmap(x + LocationList::horizontal_padding / 2,
                 y + that->get_icon_vertical_offset(), icon);

      DrawTextRect(
          x + 2 * LocationList::horizontal_padding + icon->width, y,
          ScreenWidth() - 4 * LocationList::horizontal_padding - icon->width,
          that->get_item_height(), that->item_names.at(item_index).c_str(),
          ALIGN_LEFT | VALIGN_MIDDLE);
      return 1;
    }
  } else if (action == LIST_ENDPAINT) {
    BOOST_LOG_TRIVIAL(debug) << "Finished to paint location list";
    return 1;
  } else if (action == LIST_OPEN) {
    if (0 <= item_index and item_index < that->item_names.size()) {
      BOOST_LOG_TRIVIAL(debug)
          << "Will select location from item with index " << item_index;

      SetEventHandler(application_event_handler);

      SendEvent(application_event_handler, EVT_CUSTOM,
                CustomEvent::select_location_from_list,
                static_cast<size_t>(last_selected_item_index));

      return 1;
    }
  } else if (action == LIST_MENU) {
    BOOST_LOG_TRIVIAL(debug) << "Received list menu action";
  } else if (action == LIST_DELETE) {
    BOOST_LOG_TRIVIAL(debug) << "Received list delete action";
  } else if (action == LIST_EXIT) {
    BOOST_LOG_TRIVIAL(debug) << "Received list exit action";

    SetEventHandler(application_event_handler);

    that = nullptr;
    return 1;
  } else if (action == LIST_ORIENTATION) {
    BOOST_LOG_TRIVIAL(debug) << "Changing list orientation is not supported";
  } else if (action == LIST_POINTER) {
    BOOST_LOG_TRIVIAL(debug) << "Received list pointer action";
  } else if (action == LIST_INFO) {
    BOOST_LOG_TRIVIAL(debug) << "Received list info action";
    last_selected_item_index = -1;
  } else if (action == LIST_SCROLL) {
    BOOST_LOG_TRIVIAL(debug) << "Received list scroll action";
  } else {
    BOOST_LOG_TRIVIAL(warning) << "Unexpected list action " << action;
  }
  return 0;
}
} // namespace taranis
