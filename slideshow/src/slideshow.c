#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"

#define MY_UUID {0x71, 0x90, 0x8C, 0x30, 0xA4, 0x9F, 0x11, 0xE2, 0x9E, 0x96, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66}
PBL_APP_INFO(MY_UUID, "Slideshow", "Darren Oakey", 1, 0 /* App version */, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;
TextLayer _currentDate;
TextLayer _currentTime;
BitmapLayer _currentPicture;
BmpContainer _alex, _leo, _arlene;


//
// sets up a text layer - defaulting to white on black
//
void setupTextLayer( TextLayer *layer, Window *parent, int x, int y, int width, int height, GFont font )
{	
    text_layer_init(layer, window.layer.frame);
    text_layer_set_text_color(layer, GColorWhite);
    text_layer_set_background_color(layer, GColorClear);
    layer_set_frame(&layer->layer, GRect(x, y, width, height));
    text_layer_set_font(layer, font );
    layer_add_child(&parent->layer, &layer->layer);
}

//
// sets up the display
//
void handle_init(AppContextRef ctx) {
  (void)ctx;

    window_init(&window, "Simplicity");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);
    resource_init_current_app(&APP_RESOURCES);
    
    //
    // Load the bitmaps
    //
    bmp_init_container( RESOURCE_ID_IMAGE_ALEX, &_alex);
    bmp_init_container( RESOURCE_ID_IMAGE_LEO, &_leo);
    bmp_init_container( RESOURCE_ID_IMAGE_ARLENE, &_arlene);

    //
    // create the bitmap layer at the back
    //
    bitmap_layer_init( &_currentPicture, GRect(0,0, 144, 168) );
    layer_add_child( &window.layer, &_currentPicture.layer );
    
    //
    // load the font we are using
    //
    GFont font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21));
    
    //
    // create the text layers
    //
    setupTextLayer( &_currentDate, &window, 2, 168-21, 144-4, 21, font);
    setupTextLayer( &_currentTime, &window, 2, 168-42, 144-4, 21, font );
}


//
// ticks every minute, updating the time, date and
//
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t)
{
    //
    // this seems pointless
    //
    (void)ctx;

    //
    // choose the background image - we swap every two minutes
    //
    PblTm *tickTime = t->tick_time;
    int minute = tickTime->tm_min;
    if (minute|1)
    {
        switch ((minute>>1)%3)
        {
            case 0: bitmap_layer_set_bitmap( &_currentPicture, &(_alex.bmp) ); break;
            case 1: bitmap_layer_set_bitmap( &_currentPicture, &(_leo.bmp) ); break;
            case 2: bitmap_layer_set_bitmap( &_currentPicture, &(_arlene.bmp) ); break;
        }
    }


    //
    // set the date - only changing it when the day changes
    // format strings here: http://www.gnu.org/software/emacs/manual/html_node/elisp/Time-Parsing.html
    //
    static char dateText[] = "Sun 01 September 00";
    static int lastShownDate = -1;
    int theDay = tickTime->tm_yday;
    if (theDay != lastShownDate)
    {
        lastShownDate = theDay;
        string_format_time(dateText, sizeof(dateText), "%a %B %e", tickTime );
        text_layer_set_text(&_currentDate, dateText);
    }
    
    //
    // set the time
    //
    static char timeText[] = "00:00";
    const char *timeFormat = clock_is_24h_style() ? "%R" : "%I:%M";
    string_format_time(timeText, sizeof(timeText), timeFormat, tickTime);
    text_layer_set_text(&_currentTime, timeText);

}

//
// main
//
void pbl_main(void *params)
{
    PebbleAppHandlers handlers =
    {
        .init_handler = &handle_init,
        .tick_info =
        {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }
    };
    app_event_loop(params, &handlers);
}