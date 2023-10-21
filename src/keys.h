#pragma once

#include "inkview.h"
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <memory>
#include <vector>

#include "events.h"

namespace taranis {

struct KeyEventConsumer {
protected:
  virtual ~KeyEventConsumer() {}

private:
  virtual bool handle_key_press(int key) { return 0; }

  virtual bool handle_key_repeat(int key) { return 0; }

  virtual bool handle_key_release(int key) { return 0; }

  friend struct KeyEventDispatcher;
};

struct KeyEventDispatcher {

  int handle_key_event(int event_type, int key) {
    BOOST_LOG_TRIVIAL(debug)
        << "Processing event " << format_event_type(event_type) << " for key "
        << key;

    std::shared_ptr<KeyEventConsumer> event_consumer{nullptr};
    if (event_type == EVT_KEYPRESS) {
      for (auto consumer : this->consumers) {
        if (consumer->handle_key_press(key)) {
          event_consumer = consumer;
        }
      }
    } else if (this->last_event_consumer) {
      if (event_type == EVT_KEYREPEAT) {
        if (this->last_event_consumer->handle_key_repeat(key)) {
          event_consumer = this->last_event_consumer;
        }
      } else if (event_type == EVT_KEYRELEASE) {
        if (this->last_event_consumer->handle_key_release(key)) {
          event_consumer = this->last_event_consumer;
        }
      }
    }
    if (event_consumer) {
      this->last_event_consumer = event_consumer;
      BOOST_LOG_TRIVIAL(debug) << "Last key event consumer updated";
      return 1;
    }
    BOOST_LOG_TRIVIAL(debug) << "Key event not consumed";
    this->last_event_consumer.reset();
    return 0;
  }

protected:
  void register_key_event_consumer(std::shared_ptr<KeyEventConsumer> consumer) {
    const auto found =
        std::find(this->consumers.begin(), this->consumers.end(), consumer);
    if (found == this->consumers.end()) {
      this->consumers.push_back(consumer);
    }
  }

  void
  unregister_key_event_consumer(std::shared_ptr<KeyEventConsumer> consumer) {
    const auto found =
        std::find(this->consumers.begin(), this->consumers.end(), consumer);
    if (found != this->consumers.end()) {
      this->consumers.erase(found);
      if (this->last_event_consumer == consumer) {
	this->last_event_consumer.reset();
      }
    }
  }

private:
  std::vector<std::shared_ptr<KeyEventConsumer>> consumers;

  std::shared_ptr<KeyEventConsumer> last_event_consumer;
};

} // namespace taranis
