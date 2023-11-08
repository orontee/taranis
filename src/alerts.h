#pragma once

#include <inkview.h>
#include <memory>
#include <vector>

#include "button.h"
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

  bool is_visible() const override { return not this->model->alerts.empty(); }

protected:
  void on_clicked() override;

private:
  std::shared_ptr<Model> model;
  std::shared_ptr<AlertViewer> viewer;
};

class AlertViewer : public Widget {
public:
  AlertViewer(std::shared_ptr<Model> model, std::shared_ptr<Fonts> fonts);

  void open();

  void hide();

protected:
  bool is_modal() const override { return true; }

  bool is_visible() const override { return this->visible; }

  void do_paint() override;

private:
  static constexpr int horizontal_padding{25};
  static constexpr int vertical_padding{25};

  std::shared_ptr<Model> model;
  std::shared_ptr<Fonts> fonts;

  bool visible{false};

  size_t alert_index{0};

  std::string title;
  std::string description;

  inline size_t alert_count() const { return this->model->alerts.size(); }

  void update_title();

  void update_description(const Alert &alert);

  bool handle_key_press(int key) override;

  bool handle_key_release(int key) override;
};
} // namespace taranis
