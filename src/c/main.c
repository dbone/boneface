#include <pebble.h>

static Window *s_main_window;

static TextLayer *s_time_layer;
static GFont s_time_font;

static TextLayer *s_day_layer;
static GFont s_day_font;

static TextLayer *s_month_layer;
static GFont s_month_font;

static TextLayer *s_date_layer;
static GFont s_date_font;

static void main_window_load(Window *window) {
  //s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  //s_day_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(0, 52), bounds.size.w, 50));

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int top_offset = ((bounds.size.h - 123) / 2) - 1;

  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NOVA_MONO_30));
  s_time_layer = text_layer_create(GRect(0, top_offset, bounds.size.w, 33));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  s_day_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NOVA_MONO_24));
  s_day_layer = text_layer_create(GRect(0, top_offset + 33, bounds.size.w, 30));
  text_layer_set_background_color(s_day_layer, GColorBlack);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_text(s_day_layer, "----------");
  text_layer_set_font(s_day_layer, s_day_font);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);

  s_month_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NOVA_MONO_24));
  s_month_layer = text_layer_create(GRect(0, top_offset + 63, bounds.size.w, 30));
  text_layer_set_background_color(s_month_layer, GColorBlack);
  text_layer_set_text_color(s_month_layer, GColorWhite);
  text_layer_set_text(s_month_layer, "--------");
  text_layer_set_font(s_month_layer, s_month_font);
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);

  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NOVA_MONO_24));
  s_date_layer = text_layer_create(GRect(0, top_offset + 93, bounds.size.w, 30));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "--.-----");
  text_layer_set_font(s_date_layer, s_month_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_day_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_month_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
  fonts_unload_custom_font(s_time_font);
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_day_font);
  text_layer_destroy(s_day_layer);
  fonts_unload_custom_font(s_month_font);
  text_layer_destroy(s_month_layer);
  fonts_unload_custom_font(s_date_font);
  text_layer_destroy(s_date_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[9];
  static char *t_buffer;
  static char d_buffer[11];
  static char m_buffer[9];
  static char y_buffer[8];
  static char *y_pointer;

  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M %P", tick_time);
  if (s_buffer[0] == '0') {
    t_buffer = s_buffer+1;
  } else {
    t_buffer = s_buffer;
  }
  strftime(d_buffer, sizeof(d_buffer), "%A", tick_time);
  strftime(m_buffer, sizeof(m_buffer), "%b", tick_time);
  strftime(y_buffer, sizeof(y_buffer), "%d.%Y", tick_time);
  if (y_buffer[0] == '0') {
    y_pointer = y_buffer+1;
  } else {
    y_pointer = y_buffer;
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, t_buffer);
  text_layer_set_text(s_day_layer, d_buffer);
  text_layer_set_text(s_month_layer, m_buffer);
  text_layer_set_text(s_date_layer, y_pointer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, GColorBlack);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}