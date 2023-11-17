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

class DailyForecastBox : public Widget {
public:
  DailyForecastBox(int pos_x, int pos_y, int width, int height,
                   std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
                   std::shared_ptr<Fonts> fonts);

  void do_paint() override;

private:
  static constexpr size_t row_count{8};
  static constexpr size_t column_count{9};
  static constexpr int horizontal_padding{25};

  typedef boost::variant<std::string, std::pair<std::string, std::string>,
                         ibitmap *>
      CellContent;
  // one text line, two text lines or an icon

  typedef std::array<CellContent, DailyForecastBox::row_count> ColumnContent;
  typedef std::array<ColumnContent, DailyForecastBox::column_count>
      TableContent;

  std::shared_ptr<Model> model;
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  TableContent table_content;

  int row_height;

  std::array<size_t, DailyForecastBox::column_count> column_widths;
  std::array<size_t, DailyForecastBox::column_count> column_starts;

  static constexpr size_t WeekDayColumn{0};
  static constexpr size_t IconColumn{1};
  static constexpr size_t SunHoursColumn{2};
  static constexpr size_t MorningTemperatureColumn{3};
  static constexpr size_t DayTemperatureColumn{4};
  static constexpr size_t EveningTemperatureColumn{5};
  static constexpr size_t MinMaxTemperatureColumn{6};
  static constexpr size_t WindColumn{7};
  static constexpr size_t PrecipitationColumn{8};

  static constexpr std::array<int, DailyForecastBox::column_count>
      column_text_flags{ALIGN_LEFT,  ALIGN_CENTER, ALIGN_RIGHT,
                        ALIGN_RIGHT, ALIGN_RIGHT,  ALIGN_RIGHT,
                        ALIGN_RIGHT, ALIGN_RIGHT,  ALIGN_RIGHT};

  std::pair<std::string, std::string>
  generate_precipitation_column_content(const DailyCondition &forecast) const;

  void generate_table_content();

  std::shared_ptr<ifont> get_font(size_t column_index, int line_number) const;

  int estimate_max_content_width(size_t column_index) const;

  void compute_columns_layout();

  void draw_frame() const;

  void draw_values();
};

} // namespace taranis
