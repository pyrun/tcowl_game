#ifndef ENGINE_WORLD_H
#define ENGINE_WORLD_H

#include "image.h"
#include "graphic.h"

namespace engine {
    class world : public engine::graphic_object {
        public:
            world();
            ~world();

            void begin( graphic *graphic);

            void draw( float dt, engine::graphic_draw *graphic);
        private:
            image p_image;
            graphic *p_graphic;
    };
};
#endif