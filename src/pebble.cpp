#include <pebble.h>

#include "entry.h"

static void init(void) {
  wasFirstMsg = false;
  dataInited = false;
  refreshKey = -1;
  refreshSymbol = NULL;

  app_message_init();

  char entry_title[] = "Enter Symbol";
  entry_init(entry_title);

  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  deinit();
}
