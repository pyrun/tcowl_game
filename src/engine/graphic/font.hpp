#ifndef FONT_H
#define FONT_H

#include <engine/graphic/graphic.hpp>
#include <engine/graphic/image.hpp>

#define ENGINE_FONT_OFFSET 0x20

namespace engine {
    struct font_setting {
        vec2 size;
        uint8_t breakpoint;
    };

    class font {
        public:
            font();
            ~font();

            void init( graphic *graphic, font_setting setting);

            void draw( const char *data, vec2 position);
            void print( vec2 position, const char *fmt, ...);

            void draw_character( char character, vec2 position);
        private:
            image p_image;
            graphic *p_graphic;
            font_setting p_setting;
    };
};

#endif