#include "stack.h"

namespace taranis {

Stack::Stack() : current{this->children.end()} {}

void Stack::add(const std::string &name, std::shared_ptr<Widget> child) {
  if (name.empty() or not child) {
    return;
  }

  const auto [position, inserted] =
      this->children.insert_or_assign(name, child);
  if (inserted) {
    BOOST_LOG_TRIVIAL(debug) << "New stack child, " << name;
  } else {
    BOOST_LOG_TRIVIAL(debug) << "Replaced stack child, " << name;
  }

  if (this->children.size() == 1) {
    this->set_bounding_box(child->get_pos_x(), child->get_pos_y(),
                           child->get_width(), child->get_height());
  } else {
    const auto min_x = std::min(this->get_pos_x(), child->get_pos_x());
    const auto max_x = std::max(this->get_pos_x() + this->get_width(),
                                child->get_pos_x() + child->get_width());
    const auto min_y = std::min(this->get_pos_y(), child->get_pos_y());
    const auto max_y = std::max(this->get_pos_y() + this->get_height(),
                                child->get_pos_y() + child->get_height());
    this->set_bounding_box(min_x, min_y, max_x - min_x, max_y - min_y);
  }
  if (this->current == this->children.end()) {
    this->current = position;
  }
}

std::string Stack::current_widget_name() const {
  if (this->current == this->children.end()) {
    return {};
  }
  return this->current->first;
}

std::shared_ptr<Widget> Stack::current_widget() const {
  if (this->current == this->children.end()) {
    return {};
  }
  return this->current->second;
}

bool Stack::set_current_widget_maybe(const std::string &name) {
  const auto found = this->children.find(name);
  if (found == this->children.end()) {
    BOOST_LOG_TRIVIAL(warning)
        << "Failed to find stack child with name " << name;
    return false;
  }
  if (this->current == found) {
    BOOST_LOG_TRIVIAL(debug) << "Current stack child is " << name;
    return false;
  }
  this->current = found;
  this->last_key_event_consumer.reset();
  BOOST_LOG_TRIVIAL(debug) << "Changed current stack child to " << name;
  return true;
}

void Stack::set_current_widget(const std::string &name) {
  if (this->set_current_widget_maybe(name)) {
    this->paint_and_update_screen();
    this->on_current_widget_changed();
  }
}

void Stack::set_current_widget(const std::shared_ptr<Widget> &widget) {
  for (auto element = this->children.begin(); element != this->children.end();
       ++element) {
    if (element->second == widget) {
      if (this->current->second == widget) {
        BOOST_LOG_TRIVIAL(debug) << "Current stack child is " << element->first;
        return;
      }
      this->current = element;
      this->last_key_event_consumer.reset();
      BOOST_LOG_TRIVIAL(debug)
          << "Changed current stack child to " << element->first;
      this->paint_and_update_screen();
      this->on_current_widget_changed();
      return;
    }
  }
  BOOST_LOG_TRIVIAL(warning)
      << "Failed to find stack child corresponding to given widget";
}

void Stack::set_current_widget_callback(CurrentWidgetChangedCallback callback) {
  this->current_widget_changed_callback = callback;
}

bool Stack::is_enabled() const {
  if (this->current == this->children.end()) {
    return false;
  }
  return this->current->second->is_enabled();
}

void Stack::do_paint() {
  if (this->current == this->children.end()) {
    return;
  }
  auto current_child = this->current->second;
  current_child->do_paint();
}

int Stack::handle_pointer_event(int event_type, int pointer_pos_x,
                                int pointer_pos_y) {
  if (this->current == this->children.end()) {
    return 0;
  }
  return this->current->second->handle_pointer_event(event_type, pointer_pos_x,
                                                     pointer_pos_y);
}

bool Stack::handle_key_press(int key) {
  if (this->current == this->children.end()) {
    return false;
  }
  if (this->current->second->handle_key_press(key)) {
    this->last_key_event_consumer = this->current->second;
    return true;
  };
  this->last_key_event_consumer.reset();
  return false;
}

bool Stack::handle_key_repeat(int key) {
  if (this->current == this->children.end()) {
    return false;
  }
  if (this->last_key_event_consumer == this->current->second) {
    if (this->current->second->handle_key_repeat(key)) {
      return true;
    }
    this->last_key_event_consumer.reset();
  }
  return false;
}

bool Stack::handle_key_release(int key) {
  if (this->current == this->children.end()) {
    return false;
  }
  if (this->last_key_event_consumer == this->current->second) {
    if (this->current->second->handle_key_release(key)) {
      return true;
    }
    this->last_key_event_consumer.reset();
  }
  return false;
}

void Stack::on_current_widget_changed() {
  if (this->current_widget_changed_callback) {
    this->current_widget_changed_callback();
  }
}
} // namespace taranis
