#pragma once

namespace taranis {
struct Activatable {
  Activatable() {}

  virtual ~Activatable() {}

  bool is_activated() const { return this->activated; }

  void activate() {
    if (this->activated) {
      return;
    }
    this->activated = true;

    this->on_activated_changed(true);
  }

  void desactivate() {
    if (not this->activated) {
      return;
    }
    this->activated = false;

    this->on_activated_changed(false);
  }

protected:
  virtual void on_activated_changed(bool activated){};

private:
  bool activated{false};
};
} // namespace taranis
