#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer;
static TextLayer *s_rctid_layer;
static GFont s_time_font;
static GFont s_rctid_font, s_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char buffer[] = "00:00";
  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "%k:%M", tick_time);
  }
  text_layer_set_text(s_time_layer, buffer);

  // Copy date into buffer from tm structure
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %d", tick_time);

  // Show the date
  text_layer_set_text(s_date_layer, date_buffer);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TIMBERS_LOGO);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(GRect(39, 38, 144, 168));

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Time Layer
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(15, 30), bounds.size.w, 50));

  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TITILLIUM_SEMIBOLD_40));
  text_layer_set_font(s_time_layer, s_time_font);
  
  
  //Date Layer
  s_date_layer = text_layer_create(GRect (70, 60, 144, 50));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_BOLD_20));
  text_layer_set_font(s_date_layer, s_date_font);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  
  //RCTID Layer  
  s_rctid_layer = text_layer_create(GRect (-20, 55, 144, 50));
  text_layer_set_background_color(s_rctid_layer, GColorClear);
  text_layer_set_text_color(s_rctid_layer, GColorBlack);
  text_layer_set_text(s_rctid_layer, "RCTID");
  
  s_rctid_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_BOLD_30));
  
  text_layer_set_font(s_rctid_layer, s_rctid_font);
  text_layer_set_text_alignment(s_rctid_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_rctid_layer));
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  //Destroy Date TextLayer
  text_layer_destroy(s_date_layer);
  fonts_unload_custom_font(s_date_font);
  
  //Unload RCTID
  text_layer_destroy(s_rctid_layer);
  fonts_unload_custom_font(s_rctid_font);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set the background color
  window_set_background_color(s_main_window, GColorClear);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

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
