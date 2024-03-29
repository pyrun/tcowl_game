#ifndef LOBBY_HPP
#define LOBBY_HPP

#include <engine/graphic/graphic.hpp>
#include <engine/graphic/font.hpp>
#include <engine/input/input.hpp>
#include <engine/entity/entity.hpp>

namespace game {
    enum player_state {
        player_state_idle = 0,
        player_state_inventory,
        player_state_inventory_transfer,
    };

    struct player_move_item {
        engine::inventory_entry *item;
        engine::inventory_entry item_origin_state;
        engine::inventory_grid *origin;
        engine::vec2 pos;
    };

    class player : public engine::graphic_object {
        public:
            player();
            ~player();

            void begin( engine::font *font, engine::input *input, engine::entity_handler *entity, engine::world *world);

            void reload( engine::graphic_draw *graphic) override {}
            void draw( engine::graphic_draw *graphic) override;

            void update();
            const uint32_t getPriority() override { return 256; }
        private:
            bool tryGetItemFromInventory( engine::vec2 pos, engine::entity *entity);
            bool tryPutItemToInventory( engine::vec2 pos, engine::entity *entity);
            engine::inventory_grid_state checkItemInInventory( engine::vec2 pos, engine::entity *entity);
            void clearItemMove();
            void drawInventory( engine::graphic_draw *graphic);

        private:
            player_state p_state;
            player_move_item p_item_move;

            engine::font *p_font;
            engine::input *p_input;
            engine::entity_handler *p_entity;
            engine::world *p_world;

            engine::entity *p_player;

            // Trasfer
            engine::entity *p_transfer_target;
    };
};

#endif