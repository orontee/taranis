#include "fonts.h"

#include <boost/log/trivial.hpp>

namespace taranis {

constexpr bool use_anti_aliasing{true};

Fonts::Fonts() {
  auto default_theme_font = GetThemeFont(DEFAULTFONT, DEFAULTFONT);
  if (default_theme_font == nullptr) {
    throw std::runtime_error{"Theme doesn't define a default font!"};
  }
  const auto font_scale_factor{get_screen_dpi() / Fonts::reference_dpi};

  BOOST_LOG_TRIVIAL(debug) << "Font scale factor: " << font_scale_factor;

  const auto normal_font_size{default_theme_font->size * font_scale_factor};
  const auto big_font_size = Fonts::get_big_font_size_from(normal_font_size);
  const auto small_font_size =
      Fonts::get_small_font_size_from(normal_font_size);
  const auto tiny_font_size = Fonts::get_tiny_font_size_from(normal_font_size);

  BOOST_LOG_TRIVIAL(info) << "Selected font sizes: " << big_font_size << " "
                          << normal_font_size << " " << small_font_size << " "
                          << tiny_font_size;

  this->normal = std::shared_ptr<ifont>(
      OpenFont(DEFAULTFONT, normal_font_size, use_anti_aliasing), &CloseFont);
  this->bold = std::shared_ptr<ifont>(
      OpenFont(DEFAULTFONTB, normal_font_size, use_anti_aliasing), &CloseFont);

  this->big = std::shared_ptr<ifont>(
      OpenFont(DEFAULTFONT, big_font_size, use_anti_aliasing), &CloseFont);

  this->small = std::shared_ptr<ifont>(
      OpenFont(DEFAULTFONT, small_font_size, use_anti_aliasing), &CloseFont);
  this->small_bold = std::shared_ptr<ifont>(
      OpenFont(DEFAULTFONTB, small_font_size, use_anti_aliasing), &CloseFont);

  this->tiny = std::shared_ptr<ifont>(
      OpenFont(DEFAULTFONT, tiny_font_size, use_anti_aliasing), &CloseFont);
  this->tiny_italic = std::shared_ptr<ifont>(
      OpenFont(DEFAULTFONTI, tiny_font_size, use_anti_aliasing), &CloseFont);
}

int Fonts::get_big_font_size_from(int font_size) { return font_size * 3; }

int Fonts::get_small_font_size_from(int font_size) { return font_size * 4 / 5; }

int Fonts::get_tiny_font_size_from(int font_size) { return font_size * 3 / 5; }

} // namespace taranis
