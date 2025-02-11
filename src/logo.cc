#include "logo.h"

#include <boost/log/trivial.hpp>

#include "config.h"
#include "dailyforecastbox.h"
#include "inkview.h"
#include "statusbar.h"

namespace taranis {

Logo::Logo(std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
           std::shared_ptr<Fonts> fonts)
    : Widget{0, 0, ScreenWidth(), ScreenHeight()},
      location_box{0, 0, ScreenWidth(), model, fonts}, status_bar{model, fonts},
      forecast_box{0,
                   this->location_box.get_height() + Logo::vertical_padding,
                   ScreenWidth(),
                   ScreenHeight() - 2 * Logo::vertical_padding -
                       this->location_box.get_height() -
                       this->status_bar.get_height(),
                   model,
                   icons,
                   fonts,
                   nullptr} {}

void Logo::do_paint() {
  this->location_box.do_paint();
  this->status_bar.do_paint();
  this->forecast_box.do_paint();

  const auto screenModeInverted = IvGetScreenModeInversion();
  if (screenModeInverted) {
    InvertArea(0, 0, ScreenWidth(), ScreenHeight());
  }
}

std::string LogoGenerator::get_logo_path() {
  const auto filename =
      std::string{USEROFFLOGOPATH} + "/taranis_weather_forecast.bmp";

  return filename;
}

void LogoGenerator::generate_maybe() const {
  const auto must_generate =
      this->config->read_bool("generate_shutdown_logo", false);
  if (must_generate) {
    this->generate();
  } else {
    BOOST_LOG_TRIVIAL(debug) << "Skipping logo generation";
    if (LogoGenerator::logo_exists()) {
      this->remove();
    }
  }
}

bool LogoGenerator::logo_exists() {
  const auto filename = LogoGenerator::get_logo_path();
  struct stat buffer;
  if (iv_stat(filename.c_str(), &buffer) == 0) {
    BOOST_LOG_TRIVIAL(debug) << "Logo exists";
    return true;
  }
  BOOST_LOG_TRIVIAL(debug) << "Logo doesn't exists";
  return false;
}

void LogoGenerator::generate() const {
  BOOST_LOG_TRIVIAL(debug) << "Generating new logo";

  auto *const original_canvas = GetCanvas();
  if (not original_canvas) {
    return;
  }
  std::vector<unsigned char> data;
  data.resize(ScreenHeight() * ScreenWidth());

  icanvas canvas{ScreenWidth(),
                 ScreenHeight(),
                 ScreenWidth(),
                 original_canvas->depth,
                 0,
                 ScreenWidth(),
                 0,
                 ScreenHeight(),
                 data.data()};
  SetCanvas(&canvas);

  Logo logo{this->model, this->icons, this->fonts};
  logo.paint();

  const auto bitmap =
      BitmapFromCanvas(0, 0, ScreenWidth(), ScreenHeight(), 0, &canvas);
  const auto filename = LogoGenerator::get_logo_path();
  SaveBitmap(filename.data(), bitmap);

  SetCanvas(original_canvas);
}

void LogoGenerator::remove() const {
  BOOST_LOG_TRIVIAL(debug) << "Removing logo";

  const auto filename = LogoGenerator::get_logo_path();
  iv_unlink(filename.c_str());
}
} // namespace taranis
