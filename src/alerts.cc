#include "alerts.h"

#include <cstdint>
#include <inkview.h>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include "model.h"
#include "util.h"

namespace taranis {

void AlertsButton::on_clicked() { this->viewer->open(); }

AlertViewer::AlertViewer(std::shared_ptr<Model> model,
                         std::shared_ptr<Fonts> fonts)
    : ModalWidget{}, model{model}, fonts{fonts},
      content_width{this->bounding_box.w - 2 * AlertViewer::horizontal_padding},
      title_height{2 * this->fonts->get_small_font()->height},
      alert_title_start_y{this->title_height + AlertViewer::vertical_padding} {}

void AlertViewer::open() {
  if (this->alert_count() == 0 or this->alert_index >= this->alert_count()) {
    this->hide();
    return;
  }
  this->visible = true;

  const auto &alert = this->model->alerts.at(this->alert_index);
  this->update_title_text();
  this->update_alert_title_text(alert);
  this->update_description_text(alert);

  this->update_layout();

  AddScrolledArea(&this->scrollable_view_rectangle, true);

  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_SHOW, 0, 0);
}

void AlertViewer::hide() {
  this->visible = false;
  RemoveScrolledArea(&this->scrollable_view_rectangle);

  this->alert_index = 0;

  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_SHOW, 0, 0);
}

void AlertViewer::do_paint() {
  // title
  const auto default_font = this->fonts->get_small_font();
  SetFont(default_font.get(), BLACK);

  DrawTextRect(AlertViewer::horizontal_padding, AlertViewer::vertical_padding,
               this->content_width, this->title_height,
               this->title_text.c_str(), ALIGN_CENTER);

  DrawHorizontalSeparator(0, this->title_height, ScreenWidth(),
                          HORIZONTAL_SEPARATOR_SOLID);

  // alert title
  const auto bold_font = this->fonts->get_small_bold_font();
  SetFont(bold_font.get(), BLACK);

  const auto alert_title_height = TextRectHeight(
      this->content_width, this->alert_title_text.c_str(), ALIGN_LEFT);
  DrawTextRect(AlertViewer::horizontal_padding, alert_title_start_y,
               this->content_width, alert_title_height,
               this->alert_title_text.c_str(), ALIGN_LEFT);

  // alert description
  SetFont(default_font.get(), BLACK);

  SetClipRect(&this->scrollable_view_rectangle);

  const auto description_height = TextRectHeight(
      this->content_width, this->alert_description_text.c_str(), ALIGN_LEFT);
  DrawTextRect(AlertViewer::horizontal_padding,
               this->scrollable_view_rectangle.y + this->scrollable_view_offset,
               this->content_width, description_height,
               this->alert_description_text.c_str(), ALIGN_LEFT);

  SetClip(0, 0, ScreenWidth(), ScreenHeight());
}

void AlertViewer::update_layout() {
  const auto bold_font = this->fonts->get_small_bold_font();
  const auto default_font = this->fonts->get_small_font();

  SetFont(bold_font.get(), BLACK);

  const auto alert_title_height = TextRectHeight(
      this->content_width, this->alert_title_text.c_str(), ALIGN_LEFT);

  SetFont(default_font.get(), BLACK);

  const auto scrollable_view_start_y = this->alert_title_start_y +
                                       alert_title_height +
                                       AlertViewer::vertical_padding;
  this->scrollable_view_rectangle = iRect(
      AlertViewer::horizontal_padding, scrollable_view_start_y,
      ScreenWidth() - 2 * AlertViewer::horizontal_padding,
      ScreenHeight() - scrollable_view_start_y - AlertViewer::vertical_padding,
      0);

  this->scrollable_view_offset = 0;

  const auto description_height = TextRectHeight(
      this->content_width, this->alert_description_text.c_str(), ALIGN_LEFT);
  this->min_scrollable_view_offset =
      -std::max(0, description_height - this->scrollable_view_rectangle.h);
}

void AlertViewer::update_title_text() {
  this->title_text = std::string{GetLangText("ALERT")};
  if (this->alert_count() > 1) {
    this->title_text += (" " + std::to_string(this->alert_index + 1) + "/" +
                         std::to_string(this->alert_count()));
  }
}

void AlertViewer::update_alert_title_text(const Alert &alert) {
  this->alert_title_text = alert.event;
}

void AlertViewer::update_description_text(const Alert &alert) {
  std::stringstream description_text;
  description_text << alert.description << std::endl
                   << std::endl
                   << GetLangText("Start") << " "
                   << format_full_date(alert.start_date) << std::endl
                   << GetLangText("Duration") << " "
                   << format_duration(alert.start_date, alert.end_date)
                   << std::endl;

  if (not alert.sender.empty()) {
    description_text << GetLangText("Source") << " " << alert.sender;
  }
  this->alert_description_text = description_text.str();
}

void AlertViewer::display_previous_alert_maybe() {
  if (this->alert_index != 0) {
    --this->alert_index;

    const auto &alert = this->model->alerts.at(this->alert_index);
    this->update_title_text();
    this->update_alert_title_text(alert);
    this->update_description_text(alert);

    this->update_layout();

    this->paint_and_update_screen();
  } else {
    this->hide();
  }
}

void AlertViewer::display_next_alert_maybe() {
  if (this->alert_index + 1 < this->alert_count()) {
    ++this->alert_index;

    const auto &alert = this->model->alerts.at(this->alert_index);
    this->update_title_text();
    this->update_alert_title_text(alert);
    this->update_description_text(alert);

    this->update_layout();

    this->paint_and_update_screen();
  } else {
    this->hide();
  }
}

bool AlertViewer::handle_key_release(int key) {
  if (key == IV_KEY_PREV) {
    this->display_previous_alert_maybe();
  } else if (key == IV_KEY_NEXT) {
    this->display_next_alert_maybe();
  }
  return true;
}

int AlertViewer::handle_pointer_event(int event_type, int pointer_pos_x,
                                      int pointer_pos_y) {
  if (event_type == EVT_SCROLL) {
    auto *const scroll_area = reinterpret_cast<irect *>(pointer_pos_x);
    if (scroll_area == &this->scrollable_view_rectangle) {
      const int delta_x = pointer_pos_y >> 16;
      const int delta_y = (pointer_pos_y << 16) >> 16;
      BOOST_LOG_TRIVIAL(debug)
          << "Alert viewer received a scrolling event "
          << "delta: " << delta_x << " " << delta_y
          << " current offset: " << this->scrollable_view_offset
          << " min. offset: " << this->min_scrollable_view_offset;

      this->scrollable_view_offset =
          std::min(0, std::max(this->scrollable_view_offset + delta_y,
                               this->min_scrollable_view_offset));

      this->paint_and_update_screen();

      return 1;
    }
  }
  return 0;
}

} // namespace taranis
