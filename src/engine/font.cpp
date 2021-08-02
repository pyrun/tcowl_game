#include "font.h"

using namespace engine;

// Converts UTF8 to ASCII_256. Is kept very simple and should only allow the basic characters
// 'out' = 'in' can use the same buffer because the length of the output becomes smaller in cases
uint32_t UTF8toISO8859( char *out, const char *in, uint32_t length) {
    uint32_t l_codepoint, l_index = 0;
    if (in == NULL)
        return 0;
    while( *in != 0 && l_index < length) {
        unsigned char ch = static_cast<unsigned char>(*in);
        if (ch <= 0x7f)
            l_codepoint = ch;
        else if (ch <= 0xbf)
            l_codepoint = (l_codepoint << 6) | (ch & 0x3f);
        else if (ch <= 0xdf)
            l_codepoint = ch & 0x1f;
        else if (ch <= 0xef)
            l_codepoint = ch & 0x0f;
        else
            l_codepoint = ch & 0x07;
        ++in;
        if (((*in & 0xc0) != 0x80) && (l_codepoint <= 0x10ffff)) {
            if (l_codepoint <= 255) {
                out[l_index] = static_cast<char>(l_codepoint);
            } else {
                // The special characters are replaced by blank characters
                out[l_index] = ' ';
            }
            l_index++;
        }
    }
    out[l_index] = 0;
    return l_index;
}

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
    char l_buffer[256];
    UTF8toISO8859( l_buffer, data, 256);

    const char *t;
    int size = 0;

    vec2 l_shift;
    for (t = l_buffer; *t != '\0'; t++) {
        l_shift.x += p_setting.size.x;
        draw_character( *t, position + l_shift);
    }
}

void font::print( vec2 position, const char *fmt, ...) {
    if( p_graphic == NULL)
        return;
    
    char l_buffer_fmt[256];
    uint32_t l_length;
    va_list ap;
    va_start(ap, fmt);
    l_length = vsnprintf( l_buffer_fmt, 256, fmt, ap);
    va_end(ap);
    l_length = UTF8toISO8859( l_buffer_fmt, l_buffer_fmt, 256);
    draw( l_buffer_fmt, position);
}

void font::draw_character( char character, vec2 position) {
    uint8_t l_index = character - ENGINE_FONT_OFFSET;
    if( p_setting.breakpoint == 0) // Verhinderung Teilung 0
        return;
    p_graphic->draw( &p_image,
                    position,
                    p_setting.size,
                    vec2{ (l_index%p_setting.breakpoint)*p_setting.size.x, (l_index/p_setting.breakpoint)*p_setting.size.y});
}
