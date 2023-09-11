#pragma once

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
extern const ibitmap icon_menu;
extern const ibitmap icon_warning;
;

namespace taranis {
class Icons {
public:
  Icons()
      : icon_01n{&::icon_01n_2x}, icon_02n{&::icon_02n_2x},
        icon_03n{&::icon_03n_2x}, icon_04n{&::icon_04n_2x},
        icon_09n{&::icon_09n_2x}, icon_10n{&::icon_10n_2x},
        icon_11n{&::icon_11n_2x}, icon_13n{&::icon_13n_2x},
        icon_50n{&::icon_50n_2x}, icon_menu{&::icon_menu},
        icon_warning{&::icon_warning} {}

  ibitmap *get(const std::string &name) {
    if (name == "01d" or name == "01n") {
      return const_cast<ibitmap *>(this->icon_01n);
    }
    if (name == "02d" or name == "02n") {
      return const_cast<ibitmap *>(this->icon_02n);
    }
    if (name == "03d" or name == "03n") {
      return const_cast<ibitmap *>(this->icon_03n);
    }
    if (name == "04d" or name == "04n") {
      return const_cast<ibitmap *>(this->icon_04n);
    }
    if (name == "09d" or name == "09n") {
      return const_cast<ibitmap *>(this->icon_09n);
    }
    if (name == "10d" or name == "10n") {
      return const_cast<ibitmap *>(this->icon_10n);
    }
    if (name == "11d" or name == "11n") {
      return const_cast<ibitmap *>(this->icon_11n);
    }
    if (name == "13d" or name == "13n") {
      return const_cast<ibitmap *>(this->icon_13n);
    }
    if (name == "50d" or name == "50n") {
      return const_cast<ibitmap *>(this->icon_50n);
    }
    if (name == "menu") {
      return const_cast<ibitmap *>(this->icon_menu);
    }
    if (name == "warning") {
      return const_cast<ibitmap *>(this->icon_warning);
    }
    return nullptr;
  }

private:
  const ibitmap *const icon_01n;
  const ibitmap *const icon_02n;
  const ibitmap *const icon_03n;
  const ibitmap *const icon_04n;
  const ibitmap *const icon_09n;
  const ibitmap *const icon_10n;
  const ibitmap *const icon_11n;
  const ibitmap *const icon_13n;
  const ibitmap *const icon_50n;
  const ibitmap *const icon_menu;
  const ibitmap *const icon_warning;
};
} // namespace taranis
