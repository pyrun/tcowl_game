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
        player_state_battle,
    };

    struct player_move_item {
        engine::inventory_entry *item;
        engine::inventory_entry item_origin_state;
        engine::inventory_grid *origin;
        engine::vec2 pos;
    };

    enum player_battle_team : uint8_t {
        player_battle_team_ally = 0,
        player_battle_team_enemy
    };

    struct player_battle_obj {
        engine::entity *entity;
        uint8_t team;
        uint32_t time = 0;
        uint32_t tick = 0;

        int32_t draw_index; // is set in the draw call
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
            void drawBattle( engine::graphic_draw *graphic);

            void addBattleTarget( player_battle_obj obj);
        private:
            player_state p_state;
            player_move_item p_item_move;

            engine::font *p_font;
            engine::input *p_input;
            engine::entity_handler *p_entity;
            engine::world *p_world;

            engine::entity *p_player;

            // Trasfer
            engine::image *p_backpack;
            engine::entity *p_transfer_target;

            // Battle
            std::vector<player_battle_obj> p_battle;
    };
};

#endif