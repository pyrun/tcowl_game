#ifndef LOBBY_H
#define LOBBY_H

#include "../engine/graphic.h"
#include "../engine/font.h"
#include "../engine/input.h"
#include "../engine/entity.h"


namespace game {
    class lobby : public engine::graphic_object {
        public:
            lobby();
            ~lobby();

            void init( engine::font *font, engine::input_map *input, engine::entity_handler *entity);

            void draw( engine::graphic_draw *graphic);

            void update();
        private:
            engine::font *p_font;
            engine::input_map *p_input;
            engine::entity_handler *p_entity;
    };
};

#endif