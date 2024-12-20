#include "forecast.h"

#include "dailyforecastbox.h"
#include "events.h"
#include "pager.h"
#include <memory>

namespace taranis {

ForecastStack::ForecastStack(
    int pos_x, int pos_y, int width, int height, std::shared_ptr<Model> model,
    std::shared_ptr<Icons> icons, std::shared_ptr<Fonts> fonts,
    std::shared_ptr<DailyForecastViewer> daily_forecast_viewer)
    : Widget{pos_x, pos_y, width, height}, model{model},
      pager{std::make_shared<DotPager>(
          pos_x, pos_y + height - ForecastStack::pager_height, width,
          ForecastStack::pager_height, std::shared_ptr<Paginated>(this))},
      stack{std::make_shared<Stack>()} {
  const auto stack_height =
      height - ForecastStack::pager_height - ForecastStack::vertical_padding;

  this->hourly_forecast_box = std::make_shared<HourlyForecastBox>(
      pos_x, pos_y, width, stack_height, model, icons, fonts);

  auto daily_forecast_box = std::make_shared<DailyForecastBox>(
      pos_x, pos_y, width, stack_height, model, icons, fonts,
      daily_forecast_viewer);

  this->stack->add("hourly-forecast", this->hourly_forecast_box);
  this->stack->add("daily-forecast", daily_forecast_box);

  const auto pager_update_callback = [=]() {
    this->pager->paint_and_update_screen();
  };
  this->hourly_forecast_box->set_page_changed_callback(pager_update_callback);
  this->stack->set_current_widget_callback(pager_update_callback);

  this->swipe_detector.set_bounding_box(this->get_bounding_box());

  auto forecast_to_display = this->model->display_daily_forecast
                                 ? "daily-forecast"
                                 : "hourly-forecast";
  this->stack->set_current_widget_maybe(forecast_to_display);
}

int ForecastStack::handle_pointer_event(int event_type, int pointer_pos_x,
                                        int pointer_pos_y) {
  if (this->handle_possible_swipe(event_type, pointer_pos_x, pointer_pos_y)) {
    return 1;
  }
  return this->stack->handle_pointer_event(event_type, pointer_pos_x,
                                           pointer_pos_y);
}

bool ForecastStack::handle_possible_swipe(int event_type, int pointer_pos_x,
                                          int pointer_pos_y) {
  const auto swipe = this->swipe_detector.guess_event_swipe_type(
      event_type, pointer_pos_x, pointer_pos_y);
  if (swipe != SwipeType::no_swipe) {
    const auto current_forecast_name = this->stack->current_widget_name();
    if (current_forecast_name == "daily-forecast") {
      if (swipe == SwipeType::left_swipe) {
        this->hourly_forecast_box->set_min_forecast_offset();
      } else if (swipe == SwipeType::right_swipe) {
        this->hourly_forecast_box->set_max_forecast_offset();
      }
      this->request_forecast_switch();
      return true;
    } else if (current_forecast_name == "hourly-forecast") {
      if (swipe == SwipeType::left_swipe) {
        if (this->hourly_forecast_box->get_forecast_offset() ==
            this->hourly_forecast_box->get_max_forecast_offset()) {
          this->request_forecast_switch();
        } else {
          this->hourly_forecast_box->increase_forecast_offset();
        }
        return true;
      } else if (swipe == SwipeType::right_swipe) {
        if (this->hourly_forecast_box->get_forecast_offset() ==
            this->hourly_forecast_box->get_min_forecast_offset()) {
          this->request_forecast_switch();
        } else {
          this->hourly_forecast_box->decrease_forecast_offset();
        }
        return true;
      }
    }
  }
  return false;
}

bool ForecastStack::handle_key_press(int key) {
  this->skip_next_key_release = false;
  return (key == IV_KEY_PREV or key == IV_KEY_NEXT);
}

bool ForecastStack::handle_key_repeat(int key) {
  const auto current_forecast_name = this->stack->current_widget_name();
  if (current_forecast_name == "hourly-forecast") {
    this->prepare_child_switch_by_key(key);
    this->request_forecast_switch();
  }
  return true;
}

bool ForecastStack::handle_key_release(int key) {
  const auto current_forecast_name = this->stack->current_widget_name();
  if (current_forecast_name == "hourly-forecast") {
    if ((key == IV_KEY_PREV and
         this->hourly_forecast_box->get_forecast_offset() ==
             this->hourly_forecast_box->get_min_forecast_offset()) or
        (key == IV_KEY_NEXT and
         this->hourly_forecast_box->get_forecast_offset() ==
             this->hourly_forecast_box->get_max_forecast_offset())) {
      this->prepare_child_switch_by_key(key);
      this->request_forecast_switch();
    } else {
      this->hourly_forecast_box->handle_key_release(key);
    }
  } else {
    if (not this->skip_next_key_release) {
      this->prepare_child_switch_by_key(key);
      this->request_forecast_switch();
    }
  }
  return true;
  ;
}

size_t ForecastStack::page_count() const {
  return 1 + this->hourly_forecast_box->page_count();
}

size_t ForecastStack::current_page() const {
  const auto current_forecast_name = this->stack->current_widget_name();
  if (current_forecast_name == "hourly-forecast") {
    return this->hourly_forecast_box->current_page() + 1;
  }
  return 0;
}

void ForecastStack::do_paint() {
  this->stack->do_paint();
  this->pager->do_paint();
}

void ForecastStack::prepare_child_switch_by_key(int key) {
  const auto current_forecast_name = this->stack->current_widget_name();
  if (current_forecast_name == "daily-forecast") {
    if (key == IV_KEY_PREV) {
      this->hourly_forecast_box->set_max_forecast_offset();
    } else if (key == IV_KEY_NEXT) {
      this->hourly_forecast_box->set_min_forecast_offset();
    }
  }
  this->skip_next_key_release = true;
  // since this switch was triggered by a "key press" event
}

void ForecastStack::request_forecast_switch() const {
  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_CUSTOM,
            CustomEvent::change_daily_forecast_display, 0);
}

void ForecastStack::switch_forecast() {
  auto forecast_to_display = this->model->display_daily_forecast
                                 ? "daily-forecast"
                                 : "hourly-forecast";
  this->stack->set_current_widget(forecast_to_display);
}
} // namespace taranis
