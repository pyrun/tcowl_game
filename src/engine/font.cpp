#include "font.h"

using namespace engine;

font::font() {
    p_graphic = NULL;
    
}

font::~font() {

}

void font::init( graphic *graphic, font_setting setting) {
    p_setting = setting;
    p_graphic = graphic;
    p_image.setAlphaKey( 0, 0, 0);
    p_image.load( p_graphic, "font/font.bmp");

    p_setting.breakpoint = p_image.size.x/p_setting.size.x;
}

void font::draw( const char *data, vec2 position) {
    if( p_graphic == NULL)
        return;

    const char *t;
    int size = 0;

    vec2 l_shift;
    for (t = data; *t != '\0'; t++) {
        l_shift.x += p_setting.size.x;
        draw_character( *t, position + l_shift);
    }
}

void font::print( vec2 position, const char *fmt, ...) {
    if( p_graphic == NULL)
        return;
    
    char l_log_buffer[256];
    uint32_t l_length;
    va_list ap;
    va_start(ap, fmt);
    l_length = vsnprintf( l_log_buffer, 256, fmt, ap);
    va_end(ap);
    draw( l_log_buffer, position);
}

void font::draw_character( char character, vec2 position) {
    uint8_t l_index = character - ENGINE_FONT_OFFSET;
    if( p_setting.breakpoint == 0) // Verhinderung Teilung 0
        return;
    p_graphic->draw( &p_image,
                    position,
                    p_setting.size,
                    vec2{ (l_index%p_setting.breakpoint)*p_setting.size.x, (l_index/p_setting.breakpoint)*p_setting.size.y });
}
