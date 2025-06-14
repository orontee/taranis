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

AlertViewer::AlertViewer(int pos_y, std::shared_ptr<Model> model,
                         std::shared_ptr<Icons> icons,
                         std::shared_ptr<Fonts> fonts)
    : ModalWidget{AlertViewer::horizontal_padding, pos_y,
                  ScreenWidth() - 2 * AlertViewer::horizontal_padding,
                  ScreenHeight() - pos_y - AlertViewer::horizontal_padding},
      model{model}, icons{icons}, fonts{fonts} {}

void AlertViewer::open() {
  if (this->alert_count() == 0 or this->alert_index >= this->alert_count()) {
    this->hide();
    return;
  }
  this->visible = true;

  this->update_title_text();

  const auto &alert = this->model->alerts.at(this->alert_index);
  this->update_alert_title_text(alert);
  this->update_description_text(alert);

  this->identify_scrollable_area();
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
  const auto default_font = this->fonts->get_small_font();
  const auto bold_font = this->fonts->get_small_bold_font();

  // title
  SetFont(bold_font.get(), BLACK);

  const auto title_height =
      bold_font->height + 2 * AlertViewer::vertical_padding;

  DrawTextRect(
      this->get_pos_x() + AlertViewer::horizontal_padding, this->get_pos_y(),
      this->get_width() - 2 * AlertViewer::horizontal_padding, title_height,
      this->title_text.c_str(), ALIGN_LEFT | VALIGN_MIDDLE);

  if (!this->close_button) {
    const auto close_button_icon_size =
        default_font->height + AlertViewer::vertical_padding;

    this->close_button =
        std::make_shared<Button>(close_button_icon_size, "close", this->icons);
    this->close_button->set_click_handler(std::bind(&AlertViewer::hide, this));
    this->close_button->set_pos_x(this->get_width() - close_button_icon_size -
                                  AlertViewer::horizontal_padding);
    this->close_button->set_pos_y(this->get_pos_y() +
                                  AlertViewer::vertical_padding / 4);
  }
  this->close_button->do_paint();

  DrawHorizontalSeparator(this->get_pos_x() + AlertViewer::horizontal_padding,
                          this->get_pos_y() + title_height,
                          this->get_width() -
                              2 * AlertViewer::horizontal_padding,
                          HORIZONTAL_SEPARATOR_SOLID);

  SetClipRect(&this->scrollable_view_rectangle);

  int current_row_start_y = this->scrollable_view_rectangle.y +
                            AlertViewer::vertical_padding +
                            this->scrollable_view_offset;

  // alert title
  SetFont(bold_font.get(), BLACK);

  const auto alert_title_height =
      TextRectHeight(this->get_width() - 2 * AlertViewer::horizontal_padding,
                     this->alert_title_text.c_str(), ALIGN_LEFT);
  DrawTextRect(this->get_pos_x() + AlertViewer::horizontal_padding,
               current_row_start_y,
               this->get_width() - 2 * AlertViewer::horizontal_padding,
               alert_title_height, this->alert_title_text.c_str(), ALIGN_LEFT);

  // alert description
  SetFont(default_font.get(), BLACK);

  const auto description_height =
      TextRectHeight(this->get_width() - 2 * AlertViewer::horizontal_padding,
                     this->alert_description_text.c_str(), ALIGN_LEFT);
  DrawTextRect(this->get_pos_x() + AlertViewer::horizontal_padding,
               current_row_start_y + alert_title_height,
               this->get_width() - 2 * AlertViewer::horizontal_padding,
               description_height, this->alert_description_text.c_str(),
               ALIGN_LEFT);

  SetClip(0, 0, ScreenWidth(), ScreenHeight());

  this->swipe_detector.set_bounding_box(this->bounding_box);

  DrawFrameRectCertified(this->bounding_box, THICKNESS_DEF_FOCUSED_FRAME);
}

void AlertViewer::identify_scrollable_area() {
  const auto default_font = this->fonts->get_small_font();
  const auto bold_font = this->fonts->get_small_bold_font();

  const auto scrollable_view_start_x =
      this->get_pos_x() + AlertViewer::horizontal_padding;
  const auto title_height =
      bold_font->height + 2 * AlertViewer::vertical_padding;
  const auto scrollable_view_start_y = this->get_pos_y() + title_height;

  this->scrollable_view_rectangle =
      iRect(scrollable_view_start_x, scrollable_view_start_y,
            this->get_width() - 2 * AlertViewer::horizontal_padding,
            this->get_height() - title_height, 0);

  this->scrollable_view_offset = 0;

  SetFont(bold_font.get(), BLACK);

  const auto alert_title_height =
      TextRectHeight(this->get_width() - 2 * AlertViewer::horizontal_padding,
                     this->alert_title_text.c_str(), ALIGN_LEFT);

  SetFont(default_font.get(), BLACK);

  const auto description_height =
      TextRectHeight(this->get_width() - 2 * AlertViewer::horizontal_padding,
                     this->alert_description_text.c_str(), ALIGN_LEFT);

  this->min_scrollable_view_offset =
      -std::max(0, alert_title_height + 2 * AlertViewer::vertical_padding +
                       description_height - this->scrollable_view_rectangle.h);
}

void AlertViewer::update_title_text() {
  this->title_text = std::string{GetLangText("Alert")};
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

    this->identify_scrollable_area();

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

    this->identify_scrollable_area();

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
  if (this->handle_possible_swipe(event_type, pointer_pos_x, pointer_pos_y)) {
    return 1;
  }
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
  if (this->close_button and
      this->close_button->is_in_bouding_box(pointer_pos_x, pointer_pos_y) and
      this->close_button->is_enabled()) {
    return this->close_button->handle_pointer_event(event_type, pointer_pos_x,
                                                    pointer_pos_y);
  }
  return 0;
}

bool AlertViewer::handle_possible_swipe(int event_type, int pointer_pos_x,
                                        int pointer_pos_y) {
  const auto swipe = this->swipe_detector.guess_event_swipe_type(
      event_type, pointer_pos_x, pointer_pos_y);
  if (swipe != SwipeType::no_swipe) {
    if (swipe == SwipeType::left_swipe) {
      this->display_next_alert_maybe();
    } else if (swipe == SwipeType::right_swipe) {
      this->display_previous_alert_maybe();
    }
    return true;
  }
  return false;
}

} // namespace taranis
