#include <pebble.h>

enum {
  AKEY_INT
};

static Window *window;
static TextLayer *select_layer;
static TextLayer *up_button_layer;
static TextLayer *down_button_layer;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(1, 1);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  text_layer_set_text(select_layer, "Select button tapped.");
  text_layer_set_text(up_button_layer, "");
  text_layer_set_text(down_button_layer, "");

}

void out_sent_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message sent out successfully");
}

void out_failed_handler(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Error sending message");
}

void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *text_tuple = dict_find(iter, AKEY_INT);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received a message");
  if(text_tuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", text_tuple->value->int8);
  }
}

void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Dropped message");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(1, 2);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  text_layer_set_text(select_layer, "");
  text_layer_set_text(up_button_layer, "Up button tapped.");
  text_layer_set_text(down_button_layer, "");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(1, 3);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  text_layer_set_text(select_layer, "");
  text_layer_set_text(up_button_layer, "");
  text_layer_set_text(down_button_layer, "Down button tapped.");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  select_layer = text_layer_create((GRect) { .origin = { 0, 57 }, .size = { bounds.size.w, 20 } });
  up_button_layer = text_layer_create((GRect) { .origin = { 0, 12}, .size = { bounds.size.w, 20 } });
  down_button_layer = text_layer_create((GRect) { .origin = { 0, 117}, .size = { bounds.size.w, 20 } });
  

  text_layer_set_text(up_button_layer, "Welcome");
  text_layer_set_text_alignment(up_button_layer, GTextAlignmentCenter);
  
  text_layer_set_text(select_layer, "to");
  text_layer_set_text_alignment(select_layer, GTextAlignmentCenter);

  text_layer_set_text(down_button_layer, "Elroy!");
  text_layer_set_text_alignment(down_button_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(select_layer));
  layer_add_child(window_layer, text_layer_get_layer(up_button_layer));
  layer_add_child(window_layer, text_layer_get_layer(down_button_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(select_layer);
  text_layer_destroy(up_button_layer);
  text_layer_destroy(down_button_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
