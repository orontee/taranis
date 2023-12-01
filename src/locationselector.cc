#include "locationselector.h"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <inkview.h>
#include <iomanip>
#include <sstream>

#include "events.h"
#include "util.h"

namespace {
taranis::LocationSelector *that{nullptr};
} // namespace

namespace taranis {

LocationSelector::LocationSelector(const int icon_size,
                                   std::shared_ptr<Fonts> fonts,
                                   std::shared_ptr<Icons> icons)
    : font{fonts->get_small_font()}, tiny_font{fonts->get_tiny_font()},
      radio_button_unchecked{BitmapStretchProportionally(
          icons->get("radio-button-unchecked"), icon_size, icon_size)},
      radio_button_checked{BitmapStretchProportionally(
          icons->get("radio-button-checked"), icon_size, icon_size)} {}

std::optional<Location> LocationSelector::get_location(size_t index) const {
  if (index < this->locations.size()) {
    return this->locations.at(index);
  }
  return std::experimental::nullopt;
}

void LocationSelector::set_locations(const std::vector<Location> &locations) {
  this->locations = locations;
  this->prepare_item_content();
}

void LocationSelector::open() {
  if (this->locations.size() == 0) {
    return;
  }
  this->application_event_handler = GetEventHandler();
  that = this;

  OpenList(GetLangText("Locations"), nullptr, this->get_item_width(),
           this->get_item_height(), this->locations.size(), 0,
           &handle_list_action);
}

void LocationSelector::prepare_item_content() {
  this->item_contents.clear();
  this->item_contents.reserve(this->locations.size());

  for (const auto &location : this->locations) {
    std::stringstream details_stream;
    details_stream << std::setprecision(6) << GetLangText("Longitude") << " "
                   << location.longitude << "   " << GetLangText("Latitude")
                   << " " << location.latitude;

    this->item_contents.emplace_back(format_location(location, true),
                                     details_stream.str());
  }
}

int LocationSelector::get_item_width() const {
  return ScreenWidth() - 2 * LocationSelector::horizontal_padding;
}

int LocationSelector::get_item_height() const {
  return std::max(static_cast<int>(this->radio_button_unchecked->height),
                  this->font->height + this->tiny_font->height) +
         2 * LocationSelector::vertical_padding;
}

int LocationSelector::get_icon_vertical_offset() const {
  return (get_item_height() - this->radio_button_checked->height) / 2;
}

int LocationSelector::handle_list_action(int action, int x, int y,
                                         int item_index, int state) {
  if (that == nullptr) {
    BOOST_LOG_TRIVIAL(warning)
        << "Skipping action received while uninitialized list " << action;
    return 1;
  }
  if (action == LIST_BEGINPAINT) {
    BOOST_LOG_TRIVIAL(debug) << "Starting to paint location list";
    return 1;
  } else if (action == LIST_PAINT) {
    if (0 <= item_index and item_index < that->item_contents.size()) {
      BOOST_LOG_TRIVIAL(debug)
          << "Drawing list item with index " << item_index << " and state "
          << state << " at " << x << ", " << y;

      const ibitmap *icon = that->radio_button_unchecked.get();
      if (state) {
        that->selected_item_index = item_index;
        icon = that->radio_button_checked.get();

        DrawSelection(x, y + LocationSelector::vertical_padding / 2,
                      ScreenWidth() - 2 * LocationSelector::horizontal_padding,
                      that->get_item_height() -
                          LocationSelector::vertical_padding,
                      BLACK);
      }
      DrawBitmap(x + LocationSelector::horizontal_padding / 2,
                 y + that->get_icon_vertical_offset(), icon);

      const auto second_row_height = that->tiny_font->height;

      const auto [name, details] = that->item_contents.at(item_index);

      SetFont(that->font.get(), BLACK);
      DrawTextRect(x + 2 * LocationSelector::horizontal_padding + icon->width,
                   y + LocationSelector::vertical_padding,
                   ScreenWidth() - 4 * LocationSelector::horizontal_padding -
                       icon->width,
                   that->get_item_height() - second_row_height -
                       2 * LocationSelector::vertical_padding,
                   name.c_str(), ALIGN_LEFT | VALIGN_MIDDLE);

      SetFont(that->tiny_font.get(), DGRAY);
      DrawTextRect(x + 2 * LocationSelector::horizontal_padding + icon->width,
                   y + that->get_item_height() - second_row_height -
                       LocationSelector::vertical_padding,
                   ScreenWidth() - 4 * LocationSelector::horizontal_padding -
                       icon->width,
                   second_row_height, details.c_str(),
                   ALIGN_LEFT | VALIGN_MIDDLE);

      const auto separator_y = y + that->get_item_height() - 1;
      DrawLine(x, separator_y, x + that->get_item_width(), separator_y, DGRAY);

      return 1;
    }
  } else if (action == LIST_ENDPAINT) {
    BOOST_LOG_TRIVIAL(debug) << "Finished to paint location list";
    return 1;
  } else if (action == LIST_OPEN) {
    BOOST_LOG_TRIVIAL(debug) << "Will select location from item with index "
                             << that->selected_item_index;

    SetEventHandler(that->application_event_handler);

    SendEvent(that->application_event_handler, EVT_CUSTOM,
              CustomEvent::select_location_from_list,
              static_cast<size_t>(that->selected_item_index));

    return 1;
  } else if (action == LIST_MENU) {
    BOOST_LOG_TRIVIAL(debug) << "Received list menu action";
  } else if (action == LIST_DELETE) {
    BOOST_LOG_TRIVIAL(debug) << "Received list delete action";
  } else if (action == LIST_EXIT) {
    BOOST_LOG_TRIVIAL(debug) << "Received list exit action";

    SetEventHandler(that->application_event_handler);

    that = nullptr;
    return 1;
  } else if (action == LIST_ORIENTATION) {
    BOOST_LOG_TRIVIAL(debug) << "Changing list orientation is not supported";
  } else if (action == LIST_POINTER) {
    BOOST_LOG_TRIVIAL(debug) << "Received list pointer action";
  } else if (action == LIST_INFO) {
    BOOST_LOG_TRIVIAL(debug) << "Received list info action";
    that->selected_item_index = -1;
  } else if (action == LIST_SCROLL) {
    BOOST_LOG_TRIVIAL(debug) << "Received list scroll action";
  } else {
    BOOST_LOG_TRIVIAL(warning) << "Unexpected list action " << action;
  }
  return 0;
}
} // namespace taranis
