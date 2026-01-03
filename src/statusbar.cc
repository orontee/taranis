#include "statusbar.h"

#include <experimental/optional>
#include <inkview.h>
#include <sstream>

#include "util.h"

namespace {
std::string to_string(taranis::DataProvider data_provider) {
  switch (data_provider) {
  case taranis::DataProvider::openmeteo_best_match:
    return "Open-Meteo";
  case taranis::DataProvider::openmeteo_ecmwf_ifs_hres:
    return "ECMWF";
  case taranis::DataProvider::openmeteo_kma_seamless:
    return "Open-Meteo (KMA Korea)";
  case taranis::DataProvider::openmeteo_meteofrance_seamless:
    return "Open-Meteo (Météo-France)";
  case taranis::DataProvider::openmeteo_knmi_seamless:
    return "Open-Meteo (KNMI)";
  case taranis::DataProvider::openmeteo_dmi_seamless:
    return "Open-Meteo (DMI)";
  case taranis::DataProvider::openmeteo_ukmo_seamless:
    return "Open-Meteo (UK Met Office)";
  case taranis::DataProvider::openmeteo_meteoswiss_icon_seamless:
    return "Open-Meteo (MeteoSwiss)";
  case taranis::DataProvider::openmeteo_gem_seamless:
    return "Open-Meteo (Canadian Weather Service)";
  case taranis::DataProvider::openmeteo_metno_seamless:
    return "Open-Meteo (MET Norway)";
  case taranis::DataProvider::openmeteo_italia_meteo_arpae_icon_2i:
    return "Open-Meteo (ItaliaMeteo)";
  case taranis::DataProvider::openmeteo_gfs_seamless:
    return "Open-Meteo (China Meteorological Administration)";
  case taranis::DataProvider::openmeteo_jma_seamless:
    return "Open-Meteo (JMA)";
  case taranis::DataProvider::openmeteo_bom_access_global:
    return "Open-Meteo (Australian Bureau of Meteorology)";
  case taranis::DataProvider::openweather:
    return "OpenWeather";
  }
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
