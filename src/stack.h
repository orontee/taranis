#pragma once

#include <functional>
#include <map>
#include <memory>

#include "keys.h"
#include "widget.h"

namespace taranis {
struct Stack : Widget {
  typedef std::function<void()> CurrentWidgetChangedCallback;

  Stack();

  void add(const std::string &name, std::shared_ptr<Widget> child);

  std::string current_widget_name() const;

  std::shared_ptr<Widget> current_widget() const;

  void set_current_widget(const std::string &name);

  void set_current_widget(const std::shared_ptr<Widget> &widget);

  void set_current_widget_callback(CurrentWidgetChangedCallback callback);

  bool is_enabled() const override;

  void do_paint() override;

  int handle_pointer_event(int event_type, int pointer_pos_x,
                           int pointer_pos_y) override;

  bool handle_key_press(int key) override;

  bool handle_key_repeat(int key) override;

  bool handle_key_release(int key) override;

  bool set_current_widget_maybe(const std::string &name);
  // Doesn't paint, use in constructors to set initial state

private:
  typedef std::map<std::string, std::shared_ptr<Widget>> Children;

  CurrentWidgetChangedCallback current_widget_changed_callback;

  Children children;

  Children::const_iterator current;

  std::shared_ptr<KeyEventConsumer> last_key_event_consumer{nullptr};

  void on_current_widget_changed();
};
} // namespace taranis
