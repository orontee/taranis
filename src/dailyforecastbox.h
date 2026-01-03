#pragma once

#pragma once

#include <algorithm>
#include <array>
#include <boost/variant.hpp>
#include <inkview.h>
#include <memory>
#include <numeric>

#include "fonts.h"
#include "icons.h"
#include "model.h"
#include "widget.h"

using namespace std::string_literals;

namespace taranis {

class DailyForecastViewer;

class DailyForecastBox : public Widget {
public:
  DailyForecastBox(int pos_x, int pos_y, int width, int height,
                   std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                   std::shared_ptr<Fonts> fonts,
                   std::shared_ptr<DailyForecastViewer> daily_forecast_viewer);

  void do_paint() override;

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override;

private:
  static constexpr size_t max_row_count{8};
  static constexpr int horizontal_padding{25};

  typedef boost::variant<std::string, std::pair<std::string, std::string>,
                         ibitmap *>
      CellContent;
  // one text line, two text lines or an icon

  typedef std::vector<CellContent> ColumnContent;
  typedef std::vector<ColumnContent> TableContent;

  enum ColumnType {
    WeekDayColumn,
    IconColumn,
    SunHoursColumn,
    MorningTemperatureColumn,
    DayTemperatureColumn,
    DayFeltTemperatureColumn,
    EveningTemperatureColumn,
    MinMaxTemperatureColumn,
    WindColumn,
    PrecipitationColumn,
  };

  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  std::shared_ptr<DailyForecastViewer> viewer;

  size_t column_count;
  TableContent table_content;
  int row_height;

  std::vector<size_t> column_widths;
  std::vector<size_t> column_starts;
  std::vector<ColumnType> column_types;

  std::pair<std::string, std::string>
  generate_precipitation_column_content(const DailyCondition &forecast) const;

  void generate_table_content();

  std::shared_ptr<ifont> get_font(size_t column_index, int line_number) const;

  int estimate_max_content_width(size_t column_index) const;

  void compute_columns_layout();

  void draw_frame() const;

  void draw_values();

  void on_clicked_at(int pointer_pos_x, int pointer_pos_y);

  static int column_text_flags(ColumnType type);
};
} // namespace taranis
