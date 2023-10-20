#pragma once

#include <boost/log/trivial.hpp>
#include <inkview.h>
#include <string>

extern const ibitmap icon_01n_2x;
extern const ibitmap icon_02n_2x;
extern const ibitmap icon_03n_2x;
extern const ibitmap icon_04n_2x;
extern const ibitmap icon_09n_2x;
extern const ibitmap icon_10n_2x;
extern const ibitmap icon_11n_2x;
extern const ibitmap icon_13n_2x;
extern const ibitmap icon_50n_2x;
extern const ibitmap icon_direction_0;
extern const ibitmap icon_direction_45;
extern const ibitmap icon_direction_90;
extern const ibitmap icon_direction_135;
extern const ibitmap icon_direction_180;
extern const ibitmap icon_direction_225;
extern const ibitmap icon_direction_270;
extern const ibitmap icon_direction_315;
extern const ibitmap icon_display_settings;
extern const ibitmap icon_favorite;
extern const ibitmap icon_key;
extern const ibitmap icon_measuring_tape;
extern const ibitmap icon_menu;
extern const ibitmap icon_radio_button_checked;
extern const ibitmap icon_radio_button_unchecked;
extern const ibitmap icon_taranis;
extern const ibitmap icon_warning;
;

namespace taranis {
class Icons {
public:
  ibitmap *get(const std::string &name) {
    if (name == "01d" or name == "01n") {
      return const_cast<ibitmap *>(&icon_01n_2x);
    }
    if (name == "02d" or name == "02n") {
      return const_cast<ibitmap *>(&icon_02n_2x);
    }
    if (name == "03d" or name == "03n") {
      return const_cast<ibitmap *>(&icon_03n_2x);
    }
    if (name == "04d" or name == "04n") {
      return const_cast<ibitmap *>(&icon_04n_2x);
    }
    if (name == "09d" or name == "09n") {
      return const_cast<ibitmap *>(&icon_09n_2x);
    }
    if (name == "10d" or name == "10n") {
      return const_cast<ibitmap *>(&icon_10n_2x);
    }
    if (name == "11d" or name == "11n") {
      return const_cast<ibitmap *>(&icon_11n_2x);
    }
    if (name == "13d" or name == "13n") {
      return const_cast<ibitmap *>(&icon_13n_2x);
    }
    if (name == "50d" or name == "50n") {
      return const_cast<ibitmap *>(&icon_50n_2x);
    }
    if (name == "direction-0") {
      return const_cast<ibitmap *>(&icon_direction_0);
    }
    if (name == "direction-45") {
      return const_cast<ibitmap *>(&icon_direction_45);
    }
    if (name == "direction-90") {
      return const_cast<ibitmap *>(&icon_direction_90);
    }
    if (name == "direction-135") {
      return const_cast<ibitmap *>(&icon_direction_135);
    }
    if (name == "direction-180") {
      return const_cast<ibitmap *>(&icon_direction_180);
    }
    if (name == "direction-225") {
      return const_cast<ibitmap *>(&icon_direction_225);
    }
    if (name == "direction-270") {
      return const_cast<ibitmap *>(&icon_direction_270);
    }
    if (name == "direction-315") {
      return const_cast<ibitmap *>(&icon_direction_315);
    }
    if (name == "favorite") {
      return const_cast<ibitmap *>(&icon_favorite);
    }
    if (name == "menu") {
      return const_cast<ibitmap *>(&icon_menu);
    }
    if (name == "radio-button-checked") {
      return const_cast<ibitmap *>(&icon_radio_button_checked);
    }
    if (name == "radio-button-unchecked") {
      return const_cast<ibitmap *>(&icon_radio_button_unchecked);
    }
    if (name == "warning") {
      return const_cast<ibitmap *>(&icon_warning);
    }
    BOOST_LOG_TRIVIAL(error) << "Unknown icon name " << name;

    return nullptr;
  }
};
} // namespace taranis
