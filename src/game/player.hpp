#ifndef LOBBY_HPP
#define LOBBY_HPP

#include <engine/graphic/graphic.hpp>
#include <engine/graphic/font.hpp>
#include <engine/input/input.hpp>
#include <engine/entity/entity.hpp>

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
            void drawInventory( engine::graphic_draw *graphic);
        private:
            engine::font *p_font;
            engine::input *p_input;
            engine::entity_handler *p_entity;
            engine::entity *p_player;
            engine::image *p_backpack;
    };
};

#endif