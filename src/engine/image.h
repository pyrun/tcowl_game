#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "graphic.h"

namespace engine {
    class image : public graphic_image{
        public:
            image();
            ~image();

            void setAlphaKey( uint8_t r, uint8_t g, uint8_t b);
            bool load( engine::graphic *graphic, std::string file);

            void clear();
        private:
            vec4 p_color_key;
    };
};

#endif