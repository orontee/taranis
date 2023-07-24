#include <inkview.h>

#include "app.h"

App app;

int event_handler(int event_type, int param_one, int param_two) {
  return app.process_loop_event(event_type, param_one, param_two);
}

int main() {
  InkViewMain(&event_handler);
  return 0;
}
