#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_day_layer;
static TextLayer *s_date_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_heart_layer;
static TextLayer *steps_layer;
static Layer *s_canvas_layer;

static GFont s_time_font;
static GFont s_info_font;

static int s_battery_level;

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  // Set the line color
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  // Set the fill color
  graphics_context_set_fill_color(ctx, GColorWhite);
  GPoint start = GPoint(0, 40 + 24 + 24);
  GPoint end = GPoint(bounds.size.w, 40 + 24 + 24);

  // Draw a line
  graphics_draw_line(ctx, start, end);
}

static void main_window_load(Window *window) {
  //s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  //s_day_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(0, 52), bounds.size.w, 50));

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int top_offset = 0; // ((bounds.size.h - 123) / 2) - 1;
  int left_offset = 0;

  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
    
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NOVA_MONO_30));
  s_info_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NOVA_MONO_20));
  
  s_time_layer = text_layer_create(GRect(left_offset, top_offset, bounds.size.w, 33));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  s_day_layer = text_layer_create(GRect(left_offset, top_offset + 34, bounds.size.w, 30));
  text_layer_set_background_color(s_day_layer, GColorBlack);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_text(s_day_layer, "----------");
  text_layer_set_font(s_day_layer, s_info_font);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);

  s_date_layer = text_layer_create(GRect(left_offset, top_offset + 34 + 24, bounds.size.w, 30));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "--------");
  text_layer_set_font(s_date_layer, s_info_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  s_battery_layer = text_layer_create(GRect(left_offset, top_offset + 43 + 24 + 24, bounds.size.w, 30));
  text_layer_set_background_color(s_battery_layer, GColorBlack);
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_text(s_battery_layer, "---%");
  text_layer_set_font(s_battery_layer, s_info_font);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  
  s_heart_layer = text_layer_create(GRect(left_offset, top_offset + 43 + 24 + 24 + 24, bounds.size.w, 30));
  text_layer_set_background_color(s_heart_layer, GColorBlack);
  text_layer_set_text_color(s_heart_layer, GColorWhite);
  text_layer_set_text(s_heart_layer, "---");
  text_layer_set_font(s_heart_layer, s_info_font);
  text_layer_set_text_alignment(s_heart_layer, GTextAlignmentCenter);
  
  steps_layer = text_layer_create(GRect(left_offset, top_offset + 43 + 24 + 24 + 24 + 24, bounds.size.w, 30));
  text_layer_set_background_color(steps_layer, GColorBlack);
  text_layer_set_text_color(steps_layer, GColorWhite);
  text_layer_set_text(steps_layer, "----------");
  text_layer_set_font(steps_layer, s_info_font);
  text_layer_set_text_alignment(steps_layer, GTextAlignmentCenter);
  
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_day_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_heart_layer));
  layer_add_child(window_layer, text_layer_get_layer(steps_layer));
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(window_layer, s_canvas_layer);
}

static void main_window_unload(Window *window) {
  fonts_unload_custom_font(s_time_font);
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_info_font);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_heart_layer);
  text_layer_destroy(steps_layer);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[9];
  static char *t_buffer;
  static char d_buffer[11];
  static char m_buffer[15];
  static char y_buffer[8];

  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M %P", tick_time);
  if (!clock_is_24h_style()) {
    if (s_buffer[0] == '0') {
      t_buffer = s_buffer+1;
    } else {
      t_buffer = s_buffer;
    }
  }
  strftime(d_buffer, sizeof(d_buffer), "%A", tick_time);
  strftime(m_buffer, sizeof(m_buffer), "%b %2d.%Y", tick_time);

  text_layer_set_text(s_time_layer, t_buffer);
  text_layer_set_text(s_day_layer, d_buffer);
  text_layer_set_text(s_date_layer, m_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void battery_handler(BatteryChargeState state) {
  s_battery_level = state.charge_percent;
  static char batt_buff[6];
  snprintf(batt_buff, 6, "%d%%", s_battery_level);
  if (state.is_charging) {
    for (int i = sizeof(batt_buff); i > 0; i--) {
      batt_buff[i] = batt_buff[i-1];
    }
    batt_buff[0] = '+';
  }
  text_layer_set_text(s_battery_layer, batt_buff);
}

static void prv_on_health_data(HealthEventType type, void *context) {
  // If the update was from the Heart Rate Monitor, query it
  if (type == HealthEventHeartRateUpdate) {
    HealthValue value = health_service_peek_current_value(HealthMetricHeartRateBPM);
    static char heart_buff[4];
    snprintf(heart_buff, 4, "%lu", (uint32_t) value);
    text_layer_set_text(s_heart_layer, heart_buff);
  }
  if (type == HealthEventMovementUpdate) {
    HealthValue value = health_service_peek_current_value(HealthMetricStepCount);
    static char buff[10];
    snprintf(buff, 10, "%lu", (uint32_t) value);
    text_layer_set_text(steps_layer, buff);    
  }
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, GColorBlack);
  window_stack_push(s_main_window, true);
  update_time();
  battery_handler(battery_state_service_peek());
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  health_service_events_subscribe(prv_on_health_data, NULL);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}