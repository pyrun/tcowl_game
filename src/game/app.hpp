#ifndef GAME_APP_HPP
#define GAME_APP_HPP

#include <engine/graphic/graphic.hpp>
#include <engine/graphic/font.hpp>
#include <engine/graphic/image.hpp>
#include <engine/log.hpp>
#include <engine/input_keyboard.hpp>
#include <engine/entity/type.hpp>
#include <engine/entity/entity.hpp>
#include <engine/json.hpp>
#include <engine/world/tile_manager.hpp>
#include <engine/world/biom_manager.hpp>
#include <engine/world/world.hpp>
#include <network/server.hpp>
#include <network/client_connection.hpp>
#include "player.hpp"

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
            game::player p_player;
            float p_framerate_cap; // ms
            uint32_t p_physics_lastime;
            uint32_t p_animation_lasttime;
    };
};

#endif