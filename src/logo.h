#pragma once

#include <inkview.h>
#include <memory>
#include <string>

#include "config.h"
#include "dailyforecastbox.h"
#include "fonts.h"
#include "icons.h"
#include "locationbox.h"
#include "model.h"
#include "statusbar.h"
#include "widget.h"

namespace taranis {

class Logo : public Widget {
public:
  Logo(std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
       std::shared_ptr<Fonts> fonts);

  void do_paint() override;

private:
  static constexpr int horizontal_padding{25};
  static constexpr int vertical_padding{25};

  LocationBox location_box;
  StatusBar status_bar;
  DailyForecastBox forecast_box;
};

struct LogoGenerator {
  LogoGenerator(std::shared_ptr<Config> config, std::shared_ptr<Model> model,
                std::shared_ptr<Icons> icons, std::shared_ptr<Fonts> fonts)
      : config{config}, model{model}, icons{icons}, fonts{fonts} {}

  void generate_maybe() const;

  static std::string get_logo_path();

  static bool logo_exists();

private:
  std::shared_ptr<Config> config;
  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  void generate() const;
  void remove() const;
};
} // namespace taranis
