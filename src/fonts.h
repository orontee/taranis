#pragma once

#include <inkview.h>
#include <memory>

namespace taranis {

class Fonts {
public:
  Fonts();

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
  constexpr static int reference_dpi{75};

  std::shared_ptr<ifont> normal;
  std::shared_ptr<ifont> bold;
  std::shared_ptr<ifont> big;
  std::shared_ptr<ifont> small;
  std::shared_ptr<ifont> small_bold;
  std::shared_ptr<ifont> tiny;
  std::shared_ptr<ifont> tiny_italic;

  static int get_big_font_size_from(int font_size);
  static int get_small_font_size_from(int font_size);
  static int get_tiny_font_size_from(int font_size);
};
} // namespace taranis
