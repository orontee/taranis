#pragma once

#include <boost/variant.hpp>
#include <functional>
#include <inkview.h>
#include <memory>
#include <tuple>
#include <vector>

#include "button.h"
#include "closebutton.h"
#include "fonts.h"
#include "icons.h"
#include "model.h"
#include "widget.h"

namespace taranis {

struct RowDescription {
  typedef boost::variant<std::string,
                         // a single string
                         std::pair<std::string, std::string>,
                         // two column string values
                         std::pair<std::string, int>>
      // an icon name and rotation degree
      Value;

  const std::string label{};
  const Value value{};
  const bool is_header{false};
  const std::string icon_name;
  const bool is_empty;
};

class DailyForecastViewer : public ModalWidget {
public:
  DailyForecastViewer(std::shared_ptr<Model> model,
                      std::shared_ptr<Icons> icons,
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
  std::shared_ptr<Icons> icons;
  std::shared_ptr<Fonts> fonts;

  std::shared_ptr<CloseButton> close_button;

  size_t forecast_index{0};

  irect scrollable_view_rectangle;
  int scrollable_view_offset{0};
  int min_scrollable_view_offset{0};

  // one text column, two text columns, three text columns, two mixed
  // columns

  typedef std::vector<RowDescription> DescriptionRows;

  DescriptionRows description_data;

  void identify_scrollable_area();

  void generate_description_data(const DailyCondition &condition);
};
} // namespace taranis
