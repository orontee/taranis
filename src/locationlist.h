#pragma once

#include <experimental/optional>
#include <inkview.h>
#include <memory>
#include <string>
#include <vector>

#include "fonts.h"
#include "icons.h"
#include "model.h"

namespace std {
template <class T> using optional = std::experimental::optional<T>;
}

namespace taranis {

class LocationList {
public:
  LocationList(int icon_size, std::shared_ptr<Fonts> fonts,
               std::shared_ptr<Icons> icons);

  std::optional<Location> get_location(size_t index) const;

  void set_locations(const std::vector<Location> &locations);

  void show();

private:
  static constexpr int horizontal_padding{25};
  static constexpr int vertical_padding{25};

  std::vector<Location> locations;

  std::vector<std::string> item_names;

  std::shared_ptr<ifont> font;
  const ibitmap *const radio_button_unchecked;
  const ibitmap *const radio_button_checked;

  int get_item_width() const;

  int get_item_height() const;

  int get_icon_vertical_offset() const;

  static int handle_list_action(int action, int x, int y, int item_index,
                                int state);
};
} // namespace taranis
