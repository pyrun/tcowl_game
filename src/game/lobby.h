#ifndef LOBBY_H
#define LOBBY_H

#include "../engine/graphic.h"
#include "../engine/font.h"
#include "../engine/input.h"

namespace game {
    class lobby : public engine::graphic_object {
        public:
            lobby();
            ~lobby();

            void init( engine::font *font, engine::input_map *input);

            void draw( engine::graphic_draw *graphic);

            void update();
        private:
            engine::font *p_font;
            engine::input_map *p_input;
    };
};

#endif