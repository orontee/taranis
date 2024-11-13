#pragma once

#include <inkview.h>
#include <memory>
#include <vector>

#include "button.h"
#include "closebutton.h"
#include "fonts.h"
#include "icons.h"
#include "model.h"

namespace taranis {

class AlertViewer;

class AlertsButton : public Button {
public:
  AlertsButton(int icon_size, std::shared_ptr<Model> model,
               std::shared_ptr<Icons> icons,
               std::shared_ptr<AlertViewer> viewer)
      : Button{icon_size, icons->get("warning")}, model{model}, viewer{viewer} {
  }

  bool is_enabled() const override { return not this->model->alerts.empty(); }

  bool is_visible() const override { return not this->model->alerts.empty(); }

protected:
  void on_clicked() override;

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<AlertViewer> viewer;
};

class AlertViewer : public ModalWidget {
public:
  AlertViewer(std::shared_ptr<Model> model, std::shared_ptr<Icons> icons,
              std::shared_ptr<Fonts> fonts);

  void open();

  void hide();

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

  size_t alert_index{0};

  const int content_width;
  const int title_height;
  const int alert_title_start_y;

  irect scrollable_view_rectangle;
  int scrollable_view_offset{0};
  int min_scrollable_view_offset{0};

  std::string title_text;
  std::string alert_title_text;
  std::string alert_description_text;

  inline size_t alert_count() const { return this->model->alerts.size(); }

  void update_layout();

  void update_title_text();

  void update_alert_title_text(const Alert &alert);

  void update_description_text(const Alert &alert);

  void display_previous_alert_maybe();

  void display_next_alert_maybe();
};
} // namespace taranis
