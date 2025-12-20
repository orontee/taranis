#include "statusbar.h"

#include <experimental/optional>
#include <inkview.h>
#include <sstream>

#include "util.h"

namespace {
std::string to_string(taranis::DataProvider data_provider) {
  if (data_provider == taranis::openmeteo) {
    return "Open-Meteo";
  } else if (data_provider == taranis::openweather) {
    return "OpenWeather";
  }
  return "";
}
} // namespace

namespace taranis {
StatusBar::StatusBar(std::shared_ptr<Model> model, std::shared_ptr<Fonts> fonts)
    : model{model}, font{fonts->get_tiny_font()} {
  const auto height =
      2 * this->font->height + this->top_padding + this->bottom_padding;
  const auto width = ScreenWidth();
  const auto pos_x = 0;
  const auto pos_y = ScreenHeight() - height + 1;
  this->set_bounding_box(pos_x, pos_y, width, height);
}

void StatusBar::do_paint() {
  SetFont(this->font.get(), BLACK);

  std::stringstream first_row_text;
  if (this->model->refresh_date == std::experimental::nullopt) {
    first_row_text << GetLangText("Ongoing update…");
  } else {
    first_row_text << GetLangText("Last update:") << " "
                   << format_full_date(*this->model->refresh_date);

    const auto first_row_width = StringWidth(first_row_text.str().c_str());
    const auto content_width =
        this->get_width() - this->left_padding - this->right_padding;
    if (first_row_width > content_width) {
      BOOST_LOG_TRIVIAL(debug)
          << "Shortening status bar date to avoid screen overflow";

      first_row_text.str("");

      first_row_text << GetLangText("Last update:") << " "
                     << format_short_date(*this->model->refresh_date) << ", "
                     << format_time(*this->model->refresh_date);
    }
  }

  const auto first_row_x = this->left_padding;
  const auto first_row_y = this->bounding_box.y + this->top_padding;
  DrawString(first_row_x, first_row_y, first_row_text.str().c_str());

  std::stringstream second_row_text;
  const auto data_provider_name = to_string(this->model->data_provider);
  if (not data_provider_name.empty()) {
    second_row_text << GetLangText("Weather data from") << " "
                    << data_provider_name;
  }
  SetFont(this->font.get(), DGRAY);
  const auto second_row_x = this->left_padding;
  const auto second_row_y = first_row_y + this->font->height;
  DrawString(second_row_x, second_row_y, second_row_text.str().c_str());
}

} // namespace taranis
