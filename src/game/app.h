#ifndef GAME_APP_H
#define GAME_APP_H

#include "../engine/graphic.h"
#include "../engine/log.h"
#include "../engine/input_keyboard.h"
#include "../engine/image.h"
#include "../engine/font.h"
#include "../engine/type.h"
#include "../engine/entity.h"
#include "../engine/json.h"
#include "../engine/world/tile_manager.h"
#include "../engine/world/biom_manager.h"
#include "../engine/world/world.h"
#include "../network/server.h"
#include "../network/client_connection.h"
#include "lobby.h"

#define APP_CONFIG_FILE "config.json"

namespace game {
    enum app_config_network : uint32_t {
        app_config_network_offline = 0,
        app_config_network_server,
        app_config_network_client
    };

    class app {
        public:
            app();
            ~app();

            void loadAgrs( std::string file) { p_argv.load( file); }
            void begin();

            bool update();

            void close();
        private:
            // Engine stuff
            engine::graphic p_graphic;
            engine::json p_config, p_argv;
            engine::input_handler p_input;
            engine::font p_font;
            engine::font_setting p_font_setting;
            engine::type_handler p_types;
            engine::entity_handler p_entity;
            engine::tile_manager p_tileset;
            engine::biom_manager p_bioms;
            engine::world p_world;

            // network
            // TODO: -> one class
            network::service *p_network;

            // Game logic
            game::lobby p_lobby;
            float p_framerate_cap; // ms
            uint32_t p_physics_lastime;
            uint32_t p_animation_lasttime;
    };
};

#endif