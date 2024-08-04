#pragma once

#include <boost/variant.hpp>
#include <memory>
#include <tuple>

#include "fonts.h"
#include "model.h"
#include "widget.h"

namespace taranis {
class DailyForecastViewer : public ModalWidget {
public:
  DailyForecastViewer(std::shared_ptr<Model> model,
                      std::shared_ptr<Fonts> fonts);

  void open();

  void hide();

  void set_forecast_index(size_t index);

  void do_paint() override;

  bool handle_key_release(int key) override;

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override;

private:
  static constexpr int horizontal_padding{25};
  static constexpr int vertical_padding{25};

  std::shared_ptr<Model> model;
  std::shared_ptr<Fonts> fonts;

  size_t forecast_index{0};

  const int content_width;
  const int title_height;
  const int forecast_title_start_y;

  irect scrollable_view_rectangle;
  int scrollable_view_offset{0};
  int min_scrollable_view_offset{0};

  typedef boost::variant<std::string, std::pair<std::string, std::string>,
                         std::tuple<std::string, std::string, std::string>,
                         std::pair<std::string, ibitmap *>>
      CellContent;
  // one text column, two text columns, three text columns, two mixed
  // columns

  typedef std::vector<CellContent> DescriptionRows;

  DescriptionRows description_data;

  int forecast_description_height;

  std::string title_text;
  std::string forecast_title_text;

  inline size_t forecast_count() const {
    return this->model->daily_forecast.size();
  }

  void update_layout();

  void update_title_text();

  void update_forecast_title_text(const DailyCondition &condition);

  void generate_description_data(const DailyCondition &condition);

  void display_previous_forecast_maybe();

  void display_next_forecast_maybe();
};
} // namespace taranis
