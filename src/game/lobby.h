#ifndef LOBBY_H
#define LOBBY_H

#include "../engine/graphic.h"
#include "../engine/font.h"

namespace game {
    class lobby : public engine::graphic_object {
        public:
            lobby();
            ~lobby();

            void init( engine::font *font);

            void draw( engine::graphic_draw *graphic);
        private:
            engine::font *p_font;
    };
};

#endif