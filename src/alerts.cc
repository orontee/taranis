#include "alerts.h"

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include "inkview.h"
#include "model.h"
#include "util.h"

namespace taranis {

void AlertsButton::on_clicked() { this->viewer->open(); }

AlertViewer::AlertViewer(std::shared_ptr<Model> model,
                         std::shared_ptr<Fonts> fonts)
    : Widget{0, 0, ScreenWidth(), ScreenHeight()}, model{model}, fonts{fonts} {}

void AlertViewer::open() {
  if (this->alert_count() == 0 or this->alert_index >= this->alert_count()) {
    this->hide();
    return;
  }
  this->visible = true;
  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_SHOW, 0, 0);
}

void AlertViewer::hide() {
  this->visible = false;
  this->alert_index = 0;

  const auto event_handler = GetEventHandler();
  SendEvent(event_handler, EVT_SHOW, 0, 0);
}

void AlertViewer::do_paint() {
  const auto &alert = this->model->alerts.at(this->alert_index);

  this->update_title();
  this->update_description(alert);

  const auto default_font = this->fonts->get_small_font();
  SetFont(default_font.get(), BLACK);

  const auto content_width =
      (this->bounding_box.w - 2 * AlertViewer::horizontal_padding);
  const auto title_height =
      std::max(TextRectHeight(content_width, this->title.c_str(), ALIGN_CENTER),
               2 * default_font->height);
  DrawTextRect(AlertViewer::horizontal_padding, AlertViewer::vertical_padding,
               content_width, title_height, this->title.c_str(), ALIGN_CENTER);

  DrawLine(0, title_height, ScreenWidth(), title_height, BLACK);

  const auto alert_title_start_y = title_height + AlertViewer::vertical_padding;
  auto description_start_y = alert_title_start_y;

  if (not alert.event.empty()) {
    const auto bold_font = this->fonts->get_small_bold_font();
    SetFont(bold_font.get(), BLACK);

    const auto alert_title = alert.event;
    const auto alert_title_height =
        TextRectHeight(content_width, alert_title.c_str(), ALIGN_LEFT);
    DrawTextRect(AlertViewer::horizontal_padding, alert_title_start_y,
                 content_width, alert_title_height, alert_title.c_str(),
                 ALIGN_LEFT);

    SetFont(default_font.get(), BLACK);
    description_start_y += alert_title_height + AlertViewer::vertical_padding;
  }
  const auto description_height =
      TextRectHeight(content_width, this->description.c_str(), ALIGN_LEFT);
  DrawTextRect(AlertViewer::horizontal_padding, description_start_y,
               content_width, description_height, this->description.c_str(),
               ALIGN_LEFT);
}

void AlertViewer::update_title() {
  this->title = std::string{GetLangText("ALERT")};
  if (this->alert_count() > 1) {
    this->title += (" " + std::to_string(this->alert_index + 1) + "/" +
                    std::to_string(this->alert_count()));
  }
}

void AlertViewer::update_description(const Alert &alert) {
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
  this->description = description_text.str();
}

bool AlertViewer::handle_key_press(int key) {
  return (key == IV_KEY_PREV) or (key == IV_KEY_NEXT);
}

bool AlertViewer::handle_key_release(int key) {
  if (key == IV_KEY_PREV) {
    if (this->alert_index != 0) {
      --this->alert_index;
      this->paint_and_update_screen();
    } else {
      this->hide();
    }
    return true;
  } else if (key == IV_KEY_NEXT) {
    if (this->alert_index + 1 < this->alert_count()) {
      ++this->alert_index;
      this->paint_and_update_screen();
    } else {
      this->hide();
    }
    return true;
  }
  return false;
}

} // namespace taranis
