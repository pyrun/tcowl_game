#ifndef GAME_APP_H
#define GAME_APP_H

#include "../engine/graphic.h"
#include "../engine/log.h"
#include "../engine/input.h"
#include "../engine/image.h"
#include "../engine/font.h"
#include "../engine/type.h"
#include "../engine/entity.h"
#include "../engine/config.h"
#include "../engine/world/tile_manager.h"
#include "../engine/world/biom_manager.h"
#include "../engine/world/world.h"
#include "../network/server.h"
#include "../network/client_connection.h"
#include "lobby.h"

namespace game {
    enum app_config_network {
        app_config_network_offline = 0,
        app_config_network_server,
        app_config_network_client
    };

    class app {
        public:
            app();
            ~app();

            void begin();

            bool update();

            void close();
        private:
            // Engine stuff
            engine::graphic p_graphic;
            engine::config p_config;
            engine::input p_input;
            engine::font p_font;
            engine::font_setting p_font_setting;
            engine::type_handler p_types;
            engine::entity_handler p_entity;
            engine::tile_manager p_tileset;
            engine::biom_manager p_bioms;
            engine::world p_world;

            // network
            // TODO: -> one class
            network::interface *p_network;

            // Game logic
            game::lobby p_lobby;
            float p_framerate_cap; // ms
            uint32_t p_physics_lastime;
            uint32_t p_animation_lasttime;
    };
};

#endif