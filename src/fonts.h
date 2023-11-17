#pragma once

#include <inkview.h>
#include <memory>

namespace taranis {
constexpr int FONT_SIZE = 50;
constexpr int BIG_FONT_SIZE = 150;
constexpr int SMALL_FONT_SIZE = 40;
constexpr int TINY_FONT_SIZE = 30;

class Fonts {
public:
  Fonts()
      : normal{OpenFont(DEFAULTFONT, FONT_SIZE, false), &CloseFont},
        bold{OpenFont(DEFAULTFONTB, FONT_SIZE, false), &CloseFont},
        big{OpenFont(DEFAULTFONTB, BIG_FONT_SIZE, false), &CloseFont},
        small{OpenFont(DEFAULTFONT, SMALL_FONT_SIZE, false), &CloseFont},
        small_bold{OpenFont(DEFAULTFONTB, SMALL_FONT_SIZE, false), &CloseFont},
        tiny{OpenFont(DEFAULTFONT, TINY_FONT_SIZE, false), &CloseFont},
        tiny_italic{OpenFont(DEFAULTFONTI, TINY_FONT_SIZE, false), &CloseFont} {
  }

  std::shared_ptr<ifont> get_normal_font() const { return this->normal; }

  std::shared_ptr<ifont> get_bold_font() const { return this->bold; }

  std::shared_ptr<ifont> get_big_font() const { return this->big; }

  std::shared_ptr<ifont> get_small_font() const { return this->small; }

  std::shared_ptr<ifont> get_small_bold_font() const {
    return this->small_bold;
  }

  std::shared_ptr<ifont> get_tiny_font() const { return this->tiny; }

  std::shared_ptr<ifont> get_tiny_italic_font() const {
    return this->tiny_italic;
  }

private:
  std::shared_ptr<ifont> normal;
  std::shared_ptr<ifont> bold;
  std::shared_ptr<ifont> big;
  std::shared_ptr<ifont> small;
  std::shared_ptr<ifont> small_bold;
  std::shared_ptr<ifont> tiny;
  std::shared_ptr<ifont> tiny_italic;
};
} // namespace taranis
