#include <pebble.h>

int LEFT = 0;
int RIGHT = 1;
int UP = 2; 
int DOWN = 3;
int ANIM_DURATION = 2000;

static Window *s_main_window;  
static BitmapLayer *background_layer;
static GBitmap *background_image;


// MOVED THIS HERE - WAS ORIGINALLY INSIDE FUNCTION
static PropertyAnimation *s_property_animation;


// DESTROY ANIMATION FUNCTION FROM LINK I POSTED
static void destroy_property_animation(PropertyAnimation **prop_animation) {
  if (*prop_animation == NULL) {
    return;
  }

  if (animation_is_scheduled((Animation*) *prop_animation)) {
    animation_unschedule((Animation*) *prop_animation);
  }

  property_animation_destroy(*prop_animation);
  *prop_animation = NULL;
}



void animation_slide_in(Layer *layer, int delay, int direction) {

	int startX = 0, startY = 0, endX = 0, endY = 0;
	
	GRect from_frame = layer_get_frame(layer);
	endX = from_frame.origin.x;
	endY = from_frame.origin.y;
	
	GRect to_frame = GRect(endX, endY, from_frame.size.w, from_frame.size.h);
	
	if(direction==LEFT || direction==RIGHT) {
		startX = 144 + from_frame.size.w; //way off screen left
		if(direction == RIGHT) {
			startX = startX*-1; //way off screen right
		}
		from_frame.origin.x=startX;
	}
	else if(direction==UP || direction==DOWN) {
		startY = 168 + from_frame.size.h; //way off screen top
		if(direction == DOWN) {
			startY = startY*-1; //way off screen bottom
		}
		from_frame.origin.y=startY;
	}
 
	layer_set_frame(layer, from_frame);
	layer_set_hidden(layer, false);	
	
  // Create the animation
  s_property_animation = property_animation_create_layer_frame(layer, &from_frame, &to_frame);
  animation_set_duration((Animation*)s_property_animation, ANIM_DURATION);
  animation_set_delay((Animation*)s_property_animation, delay);
  animation_set_curve((Animation*)s_property_animation, AnimationCurveEaseInOut);
	

  
  // Schedule to occur ASAP with default settings
  animation_schedule((Animation*) s_property_animation);
}



static void main_window_load(Window *window) {
  
  // Setup the layers
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  background_layer = bitmap_layer_create(bounds);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
  
  background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG);
  bitmap_layer_set_bitmap(background_layer, background_image);
  
  animation_slide_in(bitmap_layer_get_layer(background_layer), 500, 2);
  
}
  

static void main_window_unload(Window *window) {
  
  layer_remove_from_parent(bitmap_layer_get_layer(background_layer));
  bitmap_layer_destroy(background_layer);
  
  gbitmap_destroy(background_image);
  background_image = NULL;
  
  
  // REMOVING THIS LINE CAUSES 64b LEAK
  destroy_property_animation(&s_property_animation);
  
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  
}

static void deinit() {
  window_destroy(s_main_window);
}

  
  
    
int main() {
  init();
  app_event_loop();
  deinit();
}