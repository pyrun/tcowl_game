#ifndef LOBBY_H
#define LOBBY_H

#include "../engine/graphic.h"
#include "../engine/font.h"
#include "../engine/input.h"
#include "../engine/entity.h"


namespace game {
    class player : public engine::graphic_object {
        public:
            player();
            ~player();

            void begin( engine::font *font, engine::input *input, engine::entity_handler *entity);

            void reload( engine::graphic_draw *graphic) override {}
            void draw( engine::graphic_draw *graphic) override;

            void update();
            const uint32_t getPriority() override { return 256; }
        private:
            engine::font *p_font;
            engine::input *p_input;
            engine::entity_handler *p_entity;
            engine::entity *p_player;
    };
};

#endif